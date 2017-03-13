#include "recentitems_tests.h"
#include "../../xpiks-qt/Models/recentitemsmodel.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"
#include "../../xpiks-qt/Models/recentfilesmodel.h"

void checkCannotPushMoreThan(Models::RecentItemsModel &recentItems) {
    int maxRecent = recentItems.getMaxRecentItems();

    for (int i = 0; i < maxRecent*2; ++i) {
        recentItems.pushItem("/directory/path/" + QString::number(i));
    }

    QCOMPARE(recentItems.rowCount(), maxRecent);
}

void RecentItemsTests::pushMoreThanXFilesTest() {
    Models::RecentFilesModel recentFiles;

    checkCannotPushMoreThan(recentFiles);
}

void RecentItemsTests::pushMoreThanXDirectoriesTest() {
    Models::RecentDirectoriesModel recentDirectories;

    checkCannotPushMoreThan(recentDirectories);
}

void checkLastPushedIsMostRecent(Models::RecentItemsModel &recentItems) {
    QString item = "/path/to/test/dir";
    recentItems.pushItem(item);

    QCOMPARE(recentItems.getLatestUsedItem(), item);

    QString item2 = "/path/to/other/dir/here";
    recentItems.pushItem(item2);

    QCOMPARE(recentItems.getLatestUsedItem(), item2);
}

void RecentItemsTests::lastPushedIsMostRecentFileTest() {
    Models::RecentFilesModel recentFiles;

    checkLastPushedIsMostRecent(recentFiles);
}

void RecentItemsTests::lastPushedIsMostRecentDirectoryTest() {
    Models::RecentDirectoriesModel recentDirectories;

    checkLastPushedIsMostRecent(recentDirectories);
}
