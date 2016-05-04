#include "recentdirectories_tests.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"

void RecentDirectoriesTests::pushMoreThanTest() {
    Models::RecentDirectoriesModel recentDirectories;
    int maxRecent = recentDirectories.getMaxRecentDirectories();

    for (int i = 0; i < maxRecent*2; ++i) {
        recentDirectories.pushDirectory("/directory/path/" + QString::number(i));
    }

    QCOMPARE(recentDirectories.rowCount(), maxRecent);
}

void RecentDirectoriesTests::lastPushedIsMostRecent() {
    Models::RecentDirectoriesModel recentDirectories;
    QString directory = "/path/to/test/dir";
    recentDirectories.pushDirectory(directory);

    QCOMPARE(recentDirectories.getLatestUsedDirectory(), directory);

    QString directory2 = "/path/to/other/dir/here";
    recentDirectories.pushDirectory(directory2);

    QCOMPARE(recentDirectories.getLatestUsedDirectory(), directory2);
}
