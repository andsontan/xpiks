/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AESQT_H
#define AESQT_H

#include <QString>
#include <QCryptographicHash>
#include <cstdio>
#include "../tiny-aes/aes.h"

namespace Encryption {

    const uint8_t iv[]  = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x5e, 0xaf };
    const int maxLength = 2048;
    uint8_t encodingBuffer[maxLength];

    QByteArray encodeText(const QString &rawText, const QString &key) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(key.toUtf8());
        QByteArray keyData = hash.result();

        QByteArray inputData = rawText.toUtf8();
        const int length = inputData.size();

        memset(encodingBuffer, 0, maxLength * sizeof(uint8_t));

        AES128_CBC_encrypt_buffer(encodingBuffer, (uint8_t*)inputData.data(), length, (uint8_t*)keyData.data(), iv);

        QByteArray result;
        result.append((char*)encodingBuffer, -1);
        return result;
    }

    QString decodeText(const QByteArray &encodedText, const QString &key) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(key.toUtf8());
        QByteArray keyData = hash.result();

        const int length = encodedText.size();
        memset(encodingBuffer, 0, maxLength * sizeof(uint8_t));

        AES128_CBC_decrypt_buffer(encodingBuffer, (uint8_t*)encodedText.data(), length, (uint8_t*)keyData.data(), iv);

        QByteArray result;
        result.append((char*)encodingBuffer, -1);
        return result;
    }
}

#endif // AESQT_H

