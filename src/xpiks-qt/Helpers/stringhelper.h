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

#ifndef STRINGHELPER
#define STRINGHELPER

#include <string>
#include <vector>
#include <QString>
#include <stdarg.h>
#include <functional>

class QString;
class QStringList;
class QByteArray;

namespace Helpers {
    void foreachWord(const QString &text,
                     const std::function<bool (const QString &word)> &pred,
                     const std::function<void (int start, int length, const QString &word)> &action);
    QString replaceWholeWords(const QString &text, const QString &replaceWhat,
                              const QString &replaceTo, Qt::CaseSensitivity caseSensitivity=Qt::CaseInsensitive);
    bool containsWholeWords(const QString &haystack, const QString &needle, Qt::CaseSensitivity caseSensitivity=Qt::CaseInsensitive);
    QString getLastNLines(const QString &text, int N);
    void splitText(const QString &text, QStringList &parts);
    int levensteinDistance(const QString &s1, const QString &s2);
    bool isUtf8(const char* const buffer);
    QString detectEncodingAndDecode(const std::string &value);
    bool is7BitAscii(const QByteArray &s);
    std::string string_format(const std::string fmt, ...);
    QString getUnitedHitsString(const QString &text, const std::vector<int> &hits, int radius);
}

#endif // STRINGHELPER

