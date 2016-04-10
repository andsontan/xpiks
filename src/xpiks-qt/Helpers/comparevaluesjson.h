#ifndef COMPAREVALUESJSON_H
#define COMPAREVALUESJSON_H

#include <QJsonValue>

namespace Helpers {
    struct CompareValuesJson
    {
        virtual bool operator()(QJsonObject &val1, QJsonObject &val2)=0;
    };
}
#endif // COMPAREVALUESJSON_H
