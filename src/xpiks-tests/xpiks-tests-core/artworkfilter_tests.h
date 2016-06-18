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
    void caseSensitiveKeywordSearchTest();
    void cantFindWithFilterDescriptionTest();
    void cantFindWithFilterTitleTest();
    void cantFindWithFilterKeywordsTest();
    void cantFindWithFilterSpecialTest();
};

#endif // ARTWORKFILTERTESTS_H
