/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "secretsmanager.h"
#include "aes-qt.h"
#include <QCryptographicHash>
#include <QTime>
#include "../Common/defines.h"

void shuffleString(QString &str) {
    qsrand(QTime::currentTime().msec());

    int length = str.length();
    for (int i = 0; i < length; ++i) {
        int nextIndex = i + (qrand() % (length - i));

        QChar temp = str[i];
        str[i] = str[nextIndex];
        str[nextIndex] = temp;
    }
}

namespace Encryption {
    SecretsManager::SecretsManager():
        m_CommandManager(NULL)
    {
        QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        shuffleString(possibleCharacters);

        QString randomString;

        int rlength = 25 + qrand() % 10;
        randomString = possibleCharacters.left(rlength);
        LOG_DEBUG << "Generated random password:" << randomString;

        m_PasswordForMasterPassword = randomString;
        m_DefaultMasterPassword = "DefaultMasterPassword";
    }

    void SecretsManager::setMasterPasswordHash(const QString &hash) {
        LOG_INTEGRATION_TESTS << hash;
        m_MasterPasswordHash = QByteArray::fromHex(hash.toLatin1());
    }

    QString SecretsManager::encodePassword(const QString &password) {
        QMutexLocker lock(&m_EncodingMutex);

        QString encodedPassword;

        if (!password.isEmpty()) {
            QString key = getKeyForEncryption();
            encodedPassword = encodeText(password, key);
        }

        return encodedPassword;
    }

    QString SecretsManager::decodePassword(const QString &encodedPassword) {
        QMutexLocker lock(&m_EncodingMutex);

        QString decodedPassword = "";

        if (!encodedPassword.isEmpty()) {
            QString key = getKeyForEncryption();
            LOG_INTEGRATION_TESTS << "key:" << key << "pswd:" << encodedPassword;
            decodedPassword = decodeText(encodedPassword, key);
        }

        return decodedPassword;
    }

    QString SecretsManager::recodePassword(const QString &encodedPassword,
                                           const QString &oldMasterPassword,
                                           const QString &newMasterPassword) {
        QMutexLocker lock(&m_EncodingMutex);
        QString decodedPassword = decodeText(encodedPassword, oldMasterPassword);
        QString newEncodedPassword = encodeText(decodedPassword, newMasterPassword);
        return newEncodedPassword;
    }

    bool SecretsManager::testMasterPassword(const QString &masterPasswordCandidate) const {
        QByteArray hashByteArray = getPasswordHash(masterPasswordCandidate);
        bool equals = hashByteArray == m_MasterPasswordHash;
        return equals;
    }

    void SecretsManager::setMasterPassword(const QString &masterPassword) {
        QMutexLocker lock(&m_EncodingMutex);
        if (!masterPassword.isEmpty()) {
            m_EncodedMasterPassword = encodeText(masterPassword, m_PasswordForMasterPassword);
            m_MasterPasswordHash = getPasswordHash(masterPassword);
        } else {
            m_EncodedMasterPassword.clear();
            m_MasterPasswordHash.clear();
        }
    }

    void SecretsManager::resetMasterPassword() {
        LOG_DEBUG <<  "#";
        QString keyForEncryption = getKeyForEncryption();
        emit beforeMasterPasswordChange(keyForEncryption, m_DefaultMasterPassword);
        m_EncodedMasterPassword.clear();
        m_MasterPasswordHash.clear();
    }

    bool SecretsManager::changeMasterPassword(bool firstTime,
                                              const QString &inputCurrMasterPassword,
                                              const QString &newMasterPassword) {
        // Q_ASSERT(firstTime == m_EncodedMasterPassword.isEmpty());
        Q_ASSERT(firstTime == inputCurrMasterPassword.isEmpty());

        LOG_INFO << "Changing master password...";

        bool changed = false;

        if (firstTime || testMasterPassword(inputCurrMasterPassword)) {
            QString &oldMasterPassword = m_DefaultMasterPassword;
            if (!firstTime) {
                oldMasterPassword = inputCurrMasterPassword;
            }

            // all encoded items being reEncoded with new masterPassword
            emit beforeMasterPasswordChange(oldMasterPassword, newMasterPassword);

            setMasterPassword(newMasterPassword);
            changed = true;
        }

        return changed;
    }

    void SecretsManager::removeMasterPassword() {
        LOG_INFO << "#";
         m_EncodedMasterPassword.clear();
         m_MasterPasswordHash.clear();
         emit afterMasterPasswordReset();
    }

    QString SecretsManager::getKeyForEncryption() const {
        QString key;

        if (m_EncodedMasterPassword.isEmpty()) {
            key = m_DefaultMasterPassword;
        } else {
            key = decodeText(m_EncodedMasterPassword, m_PasswordForMasterPassword);
        }

        return key;
    }

    QByteArray SecretsManager::getPasswordHash(const QString &password) const {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        QString data = m_DefaultMasterPassword + password;
        hash.addData(data.toUtf8());

        return hash.result();
    }
}
