#ifndef REMOVEFILESFS_TESTS_H
#define REMOVEFILESFS_TESTS_H

#include <QObject>

class RemoveFilesFsTests: public QObject
{
    Q_OBJECT
private slots:
    void removeArtworksSignals();
    void removeArtworksNumberItems();
    void removeArtworksAllItems();
    void removeVectorSmokeTest();
};

#endif // REMOVEFILESFS_TESTS_H
