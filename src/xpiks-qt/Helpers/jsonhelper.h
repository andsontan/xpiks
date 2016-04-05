#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include "../Common/defines.h"

namespace Helpers {

    void mergeJson(QJsonDocument &mergeTo, const QJsonDocument &mergeWith, int overwrite);
    void mergeArray(QJsonArray & mergeTo, const QJsonArray & mergeFrom);
}

#endif // JSONHELPER_H
