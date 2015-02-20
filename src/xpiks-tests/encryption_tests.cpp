#include "encryption_tests.h"
#include <QDebug>
#include <QTime>
#include "../xpiks-qt/Encryption/aes-qt.h"

void EncryptionTests::simpleEncodeDecodeTest() {
    QString text = QString::fromUtf8("simple text");
    QString key = QString::fromUtf8("key");
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::encodedTextNotEqualToRawTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QString encoded = Encryption::encodeText(text, key);

    QVERIFY(encoded != text);
}

void EncryptionTests::encodedNotEqualToDecodedTest()
{
    QString text = "yet another simple text";
    QString key = "some more sophisticated key q4234";
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QVERIFY(encoded != decoded);
}

void EncryptionTests::complexEncodeDecodeTest()
{
    QString text = "simple text which has end of line and consists of \n newlines and more \t symbols";
    QString key = "key with \t tabulation and whitespaces and";
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::simpleEncodeDecodeCyrrylicTest()
{
    QString text = QString::fromUtf8("простий текст");
    QString key =  QString::fromUtf8("ключ");
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::simpleEncodeDecodeUtf8Test()
{
    QString text = QString::fromUtf8("Test \u20AC");;
    QString key = QString::fromUtf8("Test \u20AC\u20AC");;
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

void EncryptionTests::realTest()
{
    QString text = "mp";
    QString key = "93b7294e86d8d9923a497364fc8148befd67f46a";
    QString encoded = Encryption::encodeText(text, key);
    QString decoded = Encryption::decodeText(encoded, key);

    QCOMPARE(decoded, text);
}

QString getRandomString(int length) {
    QByteArray qbr;
    qbr.reserve(length);
    while (length--) {
        qbr.append('a' + qrand()%26);

        if (qrand() % 11) {
            qbr.append('0' + qrand()%10);
        }

        if (qrand() % 13) {
            qbr.append('A' + qrand()%26);
        }
    }

    return QString::fromLatin1(qbr);
}

void EncryptionTests::bigRandomTest()
{
    int iterations = 1000;
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    QString key = "93b7294e86d8d9923a497364fc8148befd67f46a";

    while(iterations--) {
        QString randomString = getRandomString(qrand() % 1000);
        QString encoded = Encryption::encodeText(randomString, key);
        QString decoded = Encryption::decodeText(encoded, key);
        qDebug() << iterations << randomString.length();
        QCOMPARE(decoded, randomString);
    }
}
