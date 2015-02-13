#ifndef RC6QT_H
#define RC6QT_H

#include <QString>
#include <QCryptographicHash>
#include <cstdio>
#include "../tiny-aes/aes.h"

QByteArray encodeText(const QString &rawText, const QString &key) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(key.toUtf8());
    QByteArray keyData = hash.result();
    QByteArray inputData = rawText.toUtf8();

    const int sz = 2048;
    uint8_t cypherText[sz];
    memset(cypherText, 0, sz * sizeof(uint8_t));
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    AES128_CBC_encrypt_buffer(cypherText, (uint8_t*)inputData.data(), inputData.length(), (uint8_t*)keyData.data(), iv);

    QByteArray result((char*)cypherText, -1);
    return result;
}

QString decodeText(const QByteArray &encodedText, const QString &key) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(key.toUtf8());
    QByteArray keyData = hash.result();

    const int sz = 2048;
    uint8_t plainText[sz];
    memset(plainText, 0, sz * sizeof(uint8_t));

    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    AES128_CBC_decrypt_buffer(plainText, (uint8_t*)encodedText.data(), encodedText.length(), (uint8_t*)keyData.data(), iv);

    QByteArray result((char*)plainText, -1);

    return QString(result);
}

#endif // RC6QT_H

