#include "vectorfilenames_tests.h"
#include <QStringList>
#include <QString>
#include <string>
#include "../../xpiks-qt/Helpers/filenameshelpers.h"

void CompareLists(const QStringList &actual, const QStringList &expected) {
    bool anyDifference = false;

    anyDifference = actual.length() != expected.length();

    if (!anyDifference) {
        int length = actual.length();
        for (int i = 0; i < length; ++i) {
            if (actual[i] != expected[i]) {
                QFAIL(QString("%1 was expected but %2 found")
                      .arg(expected[i])
                      .arg(actual[i])
                      .toStdString().c_str());
            }
        }
    } else {
        QFAIL(QString("Length was different. %1 received but %2 expected")
              .arg(actual.length())
              .arg(expected.length())
              .toStdString().c_str());
    }
}

void VectorFileNamesTests::simpleFilenamesJpgTest() {
    QStringList images;
    images << "/home/file1.jpg" << "/home/file2.jpg";
    QStringList vectorsExpected;
    vectorsExpected << "/home/file1.eps" << "/home/file1.ai" << "/home/file2.eps" << "/home/file2.ai";

    QStringList vectorsActual = Helpers::convertToVectorFilenames(images);

    CompareLists(vectorsActual, vectorsExpected);
}

void VectorFileNamesTests::simpleFilenamesTiffTest() {
    QStringList images;
    images << "/home/file1.tiff" << "/home/file2.tiff";
    QStringList vectorsExpected;
    vectorsExpected << "/home/file1.eps" << "/home/file1.ai" << "/home/file2.eps" << "/home/file2.ai";

    QStringList vectorsActual = Helpers::convertToVectorFilenames(images);

    CompareLists(vectorsActual, vectorsExpected);
}

void VectorFileNamesTests::filenamesNotReplacedTest() {
    QStringList notImages;
    notImages << "/home/file1.tar.gz" << "/home/file2.tar.gz";

    QStringList vectorsExpected;
    QStringList vectorsActual = Helpers::convertToVectorFilenames(notImages);

    CompareLists(vectorsActual, vectorsExpected);
}

void VectorFileNamesTests::simpleArchivePathTest() {
#if defined(Q_OS_WIN)
    QString filepath = "C:/test/file.ai";
    QString zipPath = "C:/test/file.zip";
#else
    QString filepath = "/c/test/file.ai";
    QString zipPath = "/c/test/file.zip";
#endif

    QString processedPath = Helpers::getArchivePath(filepath);
    QCOMPARE(processedPath, zipPath);
}
