#define CBC 1

#include <QCoreApplication>
#include <QtTest/QtTest>
#include "encryption_tests.h"
#include "indicestoranges_tests.h"
#include "addcommand_tests.h"
#include "removecommand_tests.h"
#include "vectorfilenames_tests.h"
#include "artworkmetadata_tests.h"
#include "basicmetadatamodel_tests.h"
#include "combinedmodel_tests.h"
#include "stringhelpers_tests.h"
#include "keywordvalidation_tests.h"
#include "artworkrepository_tests.h"
#include "filteredmodel_tests.h"
#include "conectivityhelpers_tests.h"
#include "undoredo_tests.h"
#include "artworkfilter_tests.h"
#include "removefilesfs_tests.h"
#include "recentdirectories_tests.h"
#include "artitemsmodel_tests.h"
#include "fixspelling_tests.h"
#include "deleteoldlogstest.h"
#include "replace_tests.h"
#include "replacepreview_tests.h"
#include "deletekeywords_tests.h"

#define QTEST_CLASS(TestObject, vName, result) \
    TestObject vName; \
    result = result + QTest::qExec(&vName, argc, argv); \

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    int result = 0;

    QTEST_CLASS(EncryptionTests, et, result);
    QTEST_CLASS(IndicesToRangesTests, itrt, result);
    QTEST_CLASS(AddCommandTests, act, result);
    QTEST_CLASS(RemoveCommandTests, rct, result);
    QTEST_CLASS(VectorFileNamesTests, vfnt, result);
    QTEST_CLASS(ArtworkMetadataTests, amt, result);
    QTEST_CLASS(BasicKeywordsModelTests, bkt, result);
    QTEST_CLASS(CombinedModelTests, cmt, result);
    QTEST_CLASS(StringHelpersTests, sht, result);
    QTEST_CLASS(KeywordValidationTests, kvt, result);
    QTEST_CLASS(ArtworkRepositoryTests, art, result);
    QTEST_CLASS(FilteredModelTests, fil, result);
    QTEST_CLASS(ConectivityHelpersTests, cht, result);
    QTEST_CLASS(UndoRedoTests, urt, result);
    QTEST_CLASS(ArtworkFilterTests, aft, result);
    QTEST_CLASS(RemoveFilesFsTests, del,result);
    QTEST_CLASS(RecentDirectoriesTests, rdt, result);
    QTEST_CLASS(ArtItemsModelTests, aimt, result);
    QTEST_CLASS(FixSpellingTests, fst, result);
    QTEST_CLASS(DeleteOldLogsTest, dolt, result);
    QTEST_CLASS(ReplaceTests, rpl, result);
    QTEST_CLASS(ReplacePreviewTests, rplp, result);
    QTEST_CLASS(DeleteKeywordsTests, dkt, result);

    QThread::sleep(1);

    return result;
}
