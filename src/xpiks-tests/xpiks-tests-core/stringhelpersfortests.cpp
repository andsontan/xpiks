#include "stringhelpersfortests.h"

QString getRandomString(int length, bool exactSize) {
    if (length <= 0) { return QString(); }
    
    QString result;
    result.reserve(length);
    QLatin1String chars("/!@#$% ^&*()_+=|");
    int charsLength = chars.size() - 1;

    while (length--) {
        result.append('a' + qrand()%26);

        if (qrand() % 3 == 0) {
            result.append(chars.data()[qrand() % charsLength]);
        }

        if (qrand() % 11 == 0) {
            result.append('0' + qrand()%10);
        }

        if (qrand() % 13 == 0) {
            result.append('A' + qrand()%26);
        }
    }

    if (exactSize) {
        result.truncate(length);
    }

    return result;
}
