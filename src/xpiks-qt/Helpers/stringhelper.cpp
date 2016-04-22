/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stringhelper.h"
#include <QFile>
#include <QTextStream>
#include <QStringRef>
#include <QVector>
#include <QString>
#include <QtGlobal>
#include <vector>

namespace Helpers {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString getLastNLines(const QString &text, int N) {
        QString result;

        QVector<QStringRef> items = text.splitRef(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        int length = items.length();

        if (length > 0) {
            int startIndex = length - N;
            if (startIndex < 0) {
                startIndex = 0;
            }

            int pos = items[startIndex].position();
            result = text.right(text.length() - pos);
        } else {
            result = text;
        }

        return result;
    }

#else
    QString getLastNLines(const QString &text, int N) {
        QString result;
        QStringList lastNLines;

        QStringList items = text.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        int length = items.length();

        if (length > 0) {
            int startIndex = length - N;
            if (startIndex < 0) {
                startIndex = 0;
            }

            for (int pos = startIndex; pos < length; pos++) {
                lastNLines.append(items[pos]);
            }

            result = lastNLines.join("\n");
        } else {
            result = text;
        }

        return result;
    }

#endif

    void splitText(const QString &text, QStringList &parts) {
        int i = 0;
        int size = text.size();
        int lastStart = -1;

        while (i < size) {
            QChar c = text[i];
            if (c.isSpace() || c.isPunct()) {
                if (lastStart != -1) {
                    int wordLength = i - lastStart;
                    QString word = text.mid(lastStart, wordLength);
                    parts.append(word);

                    lastStart = -1;
                }
            } else {
                if (lastStart == -1) {
                    lastStart = i;
                }
            }

            i++;
        }

        if (lastStart != -1) {
            int wordLength = size - lastStart;
            QString word = text.mid(lastStart, wordLength);
            parts.append(word);
        }
    }

    int levensteinDistance(const QString &s1, const QString &s2) {
        const std::size_t len1 = s1.size(), len2 = s2.size();
        std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

        for (unsigned int i = 0; i < prevCol.size(); i++) {
            prevCol[i] = i;
        }

        for (unsigned int i = 0; i < len1; i++) {
            col[0] = i + 1;

            for (unsigned int j = 0; j < len2; j++) {
                col[j + 1] = std::min(
                    std::min(prevCol[1 + j] + 1, col[j] + 1),
                    prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
            }

            col.swap(prevCol);
        }

        return prevCol[len2];
    }
}
