#ifndef JSONMERGE_TESTS_H
#define JSONMERGE_TESTS_H

#include <QJsonObject>
#include <QJsonValue>
#include <QtTest/QTest>
#include "../../xpiks-qt/Helpers/comparevaluesjson.h"

class JsonMergeTests : public QObject, public Helpers::CompareValuesJson {
    Q_OBJECT

protected:
    virtual int operator() (const QJsonObject &val1, const QJsonObject &val2) {
        return (val1["name"] == val2["name"]) ? 0 : -1;
    }

private slots:
    void mergeTwoFilesObjects();
    void mergeTwoFilesStrings();
};

#endif // JSONMERGE_TESTS_H
