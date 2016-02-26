#define CBC 1

#include <QCoreApplication>
#include <QtTest/QtTest>
#include "encryption_tests.h"
#include "indicestoranges_tests.h"
#include "addcommand_tests.h"
#include "removecommand_tests.h"
#include "vectorfilenames_tests.h"
#include "artworkmetadata_tests.h"
#include "basickeywordsmodel_tests.h"
#include "combinedmodel_tests.h"
#include "stringhelpers_tests.h"
#include "keywordvalidation_tests.h"
#include "artworkrepository_tests.h"
#include "filteredmodel_tests.h"

#define QTEST_CLASS(TestObject, vName) \
    TestObject vName; \
    QTest::qExec(&vName, argc, argv); \

int main(int argc, char *argv[]) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

//    QTEST_CLASS(EncryptionTests, et);
//    QTEST_CLASS(IndicesToRangesTests, itrt);
    QTEST_CLASS(AddCommand_Tests, act);
//    QTEST_CLASS(RemoveCommand_Tests, rct);
//    QTEST_CLASS(VectorFileNamesTests, vfnt);
//    QTEST_CLASS(ArtworkMetadataTests, amt);
//    QTEST_CLASS(BasicKeywordsModelTests, bkt);
//    QTEST_CLASS(CombinedModelTests, cmt);
//    QTEST_CLASS(StringHelpersTests, sht);
//    QTEST_CLASS(KeywordValidationTests, kvt);
//    QTEST_CLASS(ArtworkRepositoryTests, art);
//    QTEST_CLASS(FilteredModelTests, fil);
    QThread::sleep(1);
    return 0;
}
