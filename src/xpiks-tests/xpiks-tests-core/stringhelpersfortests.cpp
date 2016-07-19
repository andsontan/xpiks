#include "stringhelpersfortests.h"
#include <QDebug>

QString getRandomString(int length) {
    qDebug() << "Random length:" << length;
    QByteArray qbr;
    qbr.reserve(length);
    const char chars[] = "/!@#$% \\^&*()_+=|";
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

    return QString::fromLatin1(qbr);
}
