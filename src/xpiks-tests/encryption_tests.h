#ifndef RC6_TESTS
#define RC6_TESTS

#include <QObject>
#include <QtTest/QtTest>

class EncryptionTests : public QObject
{
    Q_OBJECT
private slots:
    void simpleEncodeDecodeTest();
    void encodedTextNotEqualToRawTest();
    void encodedNotEqualToDecodedTest();
    void complexEncodeDecodeTest();
    void simpleEncodeDecodeUnicodeTest();
};

#endif // RC6_TESTS

