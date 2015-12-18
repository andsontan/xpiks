#ifndef ARTWORKREPOSITORYTESTS_H
#define ARTWORKREPOSITORYTESTS_H

#include <QtTest/QTest>

class ArtworkRepositoryTests : public QObject
{
    Q_OBJECT
private slots:
    void simpleAccountFileTest();
    void accountSameFileTest();
    void addFilesFromOneDirectoryTest();
    void addAndRemoveSameFileTest();
    void removeNotExistingFileTest();
};

#endif // ARTWORKREPOSITORYTESTS_H
