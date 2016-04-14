#ifndef JSONMERGE_TESTS_H
#define JSONMERGE_TESTS_H

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QtTest/QTest>
#include "../../xpiks-qt/Helpers/comparevaluesjson.h"

class JsonMergeTests : public QObject,public Helpers::CompareValuesJson {
    Q_OBJECT
  bool operator()(QJsonObject &val1, QJsonObject &val2){
    return ( val1["name"] == val2["name"] );
  }

  private slots:
    void mergeTwoFilesObjects();
    void mergeTwoFilesStrings();
};

#endif // JSONMERGE_TESTS_H
