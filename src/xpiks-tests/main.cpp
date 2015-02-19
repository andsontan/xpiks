#define CBC 1

#include <QCoreApplication>
#include <QtTest/QtTest>
#include "encryption_tests.h"
#include "indicestoranges_tests.h"

#define QTEST_CLASS(TestObject, vName) \
    TestObject vName; \
    QTest::qExec(&vName, argc, argv); \

int main(int argc, char *argv[])
{
    QTEST_CLASS(EncryptionTests, et);
    QTEST_CLASS(IndicesToRangesTests, itrt);
    return 0;
}
