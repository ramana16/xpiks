/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "aes-qt.h"
#include <QString>
#include <QDataStream>
#include <QCryptographicHash>
#include <cstdio>
#include <aes.h>

#define MAX_ENCRYPTION_LENGTH 2048

namespace Encryption {

    const uint8_t iv[]  = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x5e, 0xaf };

    inline int getAlignedSize(int currSize, int alignment) {
        Q_ASSERT(currSize >= 0);

        int padding = (alignment - currSize % alignment) % alignment;
        return currSize + padding;
    }

    QString encodeText(const QString &rawText, const QString &key) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(key.toUtf8());
        QByteArray keyData = hash.result();

        const ushort *rawData = rawText.utf16();
        void *rawDataVoid = (void*)rawData;
        const char *rawDataChar = static_cast<const char*>(rawDataVoid);
        QByteArray inputData;
        inputData.append(rawDataChar, rawText.size() * 2 + 1);

        const int length = inputData.size();
        int encryptionLength = getAlignedSize(length, 16);

        QByteArray encodingBuffer(encryptionLength, 0);
        inputData.resize(encryptionLength);

        AES128_CBC_encrypt_buffer((uint8_t*)encodingBuffer.data(), (uint8_t*)inputData.data(), encryptionLength, (const uint8_t*)keyData.data(), iv);

        QByteArray data(encodingBuffer.data(), encryptionLength);
        QString hex = QString::fromLatin1(data.toHex());
        return hex;
    }

    QString decodeText(const QString &hexEncodedText, const QString &key) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(key.toUtf8());
        QByteArray keyData = hash.result();

        const int length = hexEncodedText.size();
        int encryptionLength = getAlignedSize(length, 16);

        QByteArray encodingBuffer(encryptionLength, 0);

        QByteArray encodedText = QByteArray::fromHex(hexEncodedText.toLatin1());
        encodedText.resize(encryptionLength);

        AES128_CBC_decrypt_buffer((uint8_t*)encodingBuffer.data(), (uint8_t*)encodedText.data(), encryptionLength, (const uint8_t*)keyData.data(), iv);

        void *data = encodingBuffer.data();
        const ushort *decodedData = static_cast<ushort *>(data);
        QString result = QString::fromUtf16(decodedData);
        return result;
    }
}



