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
    void brandNewDirectoriesCountTest();
    void differentNewDirectoriesCountTest();
    void newFilesCountTest();
    void noNewDirectoriesCountTest();
    void noNewFilesCountTest();
    void endAccountingWithNoNewFilesTest();
    void startAccountingNewFilesEmitsTest();
    void selectFolderTest();
};

#endif // ARTWORKREPOSITORYTESTS_H
