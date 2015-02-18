#include "encryption_tests.h"
#include <QDebug>
#include "../xpiks-qt/Encryption/aes-qt.h"

void EncryptionTests::simpleEncodeDecodeTest() {
    QString text = "simple text";
    QString key = "key";
    QByteArray encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::encodedTextNotEqualToRawTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QByteArray encoded = Encryption::encodeText(text, key);

    QVERIFY(encoded != text);
}

void EncryptionTests::encodedNotEqualToDecodedTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QByteArray encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QVERIFY(encoded != decoded);
}

void EncryptionTests::complexEncodeDecodeTest()
{
    QString text = "simple text which \0 has end of line and consists of \n newlines and more \t symbols";
    QString key = "key with \t tabulation \0 and whitespaces and";
    QByteArray encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::simpleEncodeDecodeCyrrylicTest()
{
    QString text = QString::fromUtf8("простий текст");
    QString key =  QString::fromUtf8("ключ");
    QByteArray encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::simpleEncodeDecodeUtf8Test()
{
    QString text = QString::fromUtf8("Test \u20AC");;
    QString key = QString::fromUtf8("Test \u20AC\u20AC");;
    QByteArray encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}
