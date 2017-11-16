/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SECRETSMANAGER_H
#define SECRETSMANAGER_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QMutex>

namespace Commands { class CommandManager; }

namespace Encryption {
    class SecretsManager : public QObject {
        Q_OBJECT

    public:
        SecretsManager();

        void setCommandManager(Commands::CommandManager *commandManager) {
            Q_ASSERT(commandManager != NULL);
            m_CommandManager = commandManager;
        }

    public:
        void setMasterPasswordHash(const QString &hash);

    public:
        QString encodePassword(const QString &password);
        QString decodePassword(const QString &encodedPassword);
        // operation executed before setting new master password
        // old data gets reencoded with new master password
        // could be static, but is instnance method for explicity
        QString recodePassword(const QString &encodedPassword,
                                  const QString &oldMasterPassword, const QString &newMasterPassword);

    public:
        Q_INVOKABLE QString getMasterPasswordHash() const { return QString::fromLatin1(m_MasterPasswordHash.toHex()); }
        Q_INVOKABLE bool testMasterPassword(const QString &masterPasswordCandidate) const;
        Q_INVOKABLE bool isMasterPasswordSet() const { return !m_MasterPasswordHash.isEmpty(); }

    public:
        Q_INVOKABLE void setMasterPassword(const QString &masterPassword);
        Q_INVOKABLE void resetMasterPassword();
        // should be executed on close of Upload dialog
        Q_INVOKABLE void purgeMasterPassword() { m_EncodedMasterPassword.clear(); }
        Q_INVOKABLE bool changeMasterPassword(bool firstTime, const QString &inputCurrMasterPassword,
                                              const QString &newMasterPassword);
        Q_INVOKABLE void removeMasterPassword();

    signals:
        void beforeMasterPasswordChange(const QString &oldMasterPassword, const QString &newMasterPassword);
        void afterMasterPasswordReset();

    private:
        QString getKeyForEncryption() const;
        QByteArray getPasswordHash(const QString &password) const;

    private:
        // for keeping in memory master password
        QString m_PasswordForMasterPassword;
        QString m_EncodedMasterPassword;
        QString m_DefaultMasterPassword;
        // used for checks in Upload dialog and changing MP
        QByteArray m_MasterPasswordHash;
        Commands::CommandManager *m_CommandManager;
        QMutex m_EncodingMutex;
    };
}

#endif // SECRETSMANAGER_H
