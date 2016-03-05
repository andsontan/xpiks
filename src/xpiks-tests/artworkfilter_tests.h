#ifndef ARTWORKFILTERTESTS_H
#define ARTWORKFILTERTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class ArtworkFilterTests : public QObject
{
    Q_OBJECT
private slots:
    void searchImageVectorTest();
    void searchByKeywordsTest();
    void searchByTitleTest();
    void searchByDescriptionTest();
    void strictSearchTest();
};

#endif // ARTWORKFILTERTESTS_H
