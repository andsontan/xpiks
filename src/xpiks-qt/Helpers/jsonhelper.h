#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QSet>
#include "../Common/defines.h"
#include "comparevaluesjson.h"

namespace Helpers {
   void mergeJson(QJsonDocument &mergeTo, const QJsonDocument &mergeWith, int overwrite,CompareValuesJson &comparator);
}

#endif // JSONHELPER_H
