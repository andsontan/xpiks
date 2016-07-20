#include "stringhelpersfortests.h"

QString getRandomString(int length, bool exactSize) {
    if (length <= 0) { return QString(); }
    
    QByteArray qbr;
    qbr.reserve(length);
    const char chars[] = "/!@#$% ^&*()_+=|";
    int charsLength = sizeof(chars);
    while (length--) {
        qbr.append('a' + qrand()%26);

        if (qrand() % 3 == 0) {
            qbr.append(chars[qrand() % charsLength]);
        }

        if (qrand() % 11 == 0) {
            qbr.append('0' + qrand()%10);
        }

        if (qrand() % 13 == 0) {
            qbr.append('A' + qrand()%26);
        }
    }

    if (exactSize) {
        qbr.truncate(length);
    }

    return QString::fromLatin1(qbr);
}
