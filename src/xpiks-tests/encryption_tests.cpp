#include "encryption_tests.h"
#include "QDebug"
#include "../xpiks-qt/Encoding/aes-qt.h"

void EncryptionTests::simpleEncodeDecodeTest() {
    QString text = "simple text";
    QString key = "key";
    QByteArray encoded = encodeText(text, key);
    QString decoded = decodeText(encoded, key);

    QVERIFY(decoded == text);
}

void EncryptionTests::encodedTextNotEqualToRawTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QByteArray encoded = encodeText(text, key);

    QVERIFY(encoded != text);
}

void EncryptionTests::encodedNotEqualToDecodedTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QByteArray encoded = encodeText(text, key);
    QString decoded = decodeText(encoded, key);

    QVERIFY(encoded != decoded);
}

void EncryptionTests::complexEncodeDecodeTest()
{
    QString text = "simple text which consists of \n newlines and more \t symbols";
    QString key = "key with \t tabulation and whitespaces and";
    QByteArray encoded = encodeText(text, key);
    QString decoded = decodeText(encoded, key);

    QVERIFY(decoded == text);
}

void EncryptionTests::simpleEncodeDecodeCyrrylicTest()
{
    QString text = "простий текст";
    QString key = "ключ";
    QByteArray encoded = encodeText(text, key);
    QString decoded = decodeText(encoded, key);

    QVERIFY(decoded == text);
}

void EncryptionTests::simpleEncodeDecodeUtf8Test()
{
    QString text = QString::fromUtf8("Test \u20AC");;
    QString key = QString::fromUtf8("Test \u20AC\u20AC");;
    QByteArray encoded = encodeText(text, key);
    QString decoded = decodeText(encoded, key);

    QVERIFY(decoded == text);
}
