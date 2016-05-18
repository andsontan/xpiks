#ifndef ARTITEMSMODELTESTS_H
#define ARTITEMSMODELTESTS_H

#include <QObject>
#include <QtTest/QTest>

class ArtItemsModelTests : public QObject
{
    Q_OBJECT
private slots:
    void removeUnavailableTest();
    void unselectAllTest();
    void modificationChangesModifiedCountTest();
    void removeArtworkDirectorySimpleTest();
    void setAllSavedResetsModifiedCountTest();
    void removingLockedArtworksTest();
    void plainTextEditToEmptyKeywordsTest();
    void plainTextEditToOneKeywordTest();
    void plainTextEditToSeveralKeywordsTest();
    void plainTextEditToAlmostEmptyTest();
    void plainTextEditToMixedTest();
};

#endif // ARTITEMSMODELTESTS_H
