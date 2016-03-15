#ifndef REMOVEFILESFS_TESTS_H
#define REMOVEFILESFS_TESTS_H

#include <QObject>

class RemoveFilesFs_Tests: public QObject
{
    Q_OBJECT
private slots:
    void removeArtworksSignals();
    void removeArtworksNumberItems();
};

#endif // REMOVEFILESFS_TESTS_H
