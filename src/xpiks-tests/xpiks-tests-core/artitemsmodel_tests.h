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
};

#endif // ARTITEMSMODELTESTS_H
