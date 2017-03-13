#include "recentitems_tests.h"
#include "../../xpiks-qt/Models/recentitemsmodel.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"
#include "../../xpiks-qt/Models/recentfilesmodel.h"

void doPushMoreThan(Models::RecentItemsModel &recentItems) {
    int maxRecent = recentItems.getMaxRecentItems();

    for (int i = 0; i < maxRecent*2; ++i) {
        recentItems.pushItem("/directory/path/" + QString::number(i));
    }

    QCOMPARE(recentItems.rowCount(), maxRecent);
}

void RecentItemsTests::pushMoreThanTest() {
    Models::RecentDirectoriesModel recentDirectories;
    Models::RecentFilesModel recentFiles;

    doPushMoreThan(recentDirectories);
    doPushMoreThan(recentFiles);
}

void doLastPushedIsMostRecent(Models::RecentItemsModel &recentItems) {
    QString item = "/path/to/test/dir";
    recentItems.pushItem(item);

    QCOMPARE(recentItems.getLatestUsedItem(), item);

    QString item2 = "/path/to/other/dir/here";
    recentItems.pushItem(item2);

    QCOMPARE(recentItems.getLatestUsedItem(), item2);
}

void RecentItemsTests::lastPushedIsMostRecent() {
    Models::RecentDirectoriesModel recentDirectories;
    Models::RecentFilesModel recentFiles;

    doLastPushedIsMostRecent(recentDirectories);
    doLastPushedIsMostRecent(recentFiles);
}
