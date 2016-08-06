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
#include <QByteArray>
#include <QString>
#include <QtGlobal>
#include <vector>
#include <utility>
#include <algorithm>
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"

namespace Helpers {
    void foreachWord(const QString &text,
            const std::function<bool (const QString &word)> &pred,
            const std::function<void (int start, int length, const QString &word)> &action)
    {
        int i = 0;
        const int size = text.size();
        int lastStart = -1;

        while (i < size) {
            QChar c = text[i];
            if (c.isSpace() || c.isPunct()) {
                if (lastStart != -1) {
                    int wordLength = i - lastStart;
                    QString word = text.mid(lastStart, wordLength);

                    if (pred(word)) {
                        action(lastStart, wordLength, word);
                    }

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

            if (pred(word)) {
                action(lastStart, wordLength, word);
            }
        }
    }

    bool isLeftWordBound(const QString &text, int index, bool skipWordBounds=false) {
        if (index == 0) { return true; }

        QChar curr = text[index];
        QChar prev = text[index - 1];

        const bool currIsSeparator = curr.isPunct() || curr.isSpace();
        const bool prevIsSeparator = prev.isPunct() || prev.isSpace();

        return (skipWordBounds || !currIsSeparator) && (prevIsSeparator);
    }

    bool isRightWordBound(const QString &text, int lastIndex, int index, bool skipWordBounds=false) {
        if (index == lastIndex) { return true; }

        QChar curr = text[index];
        QChar next = text[index + 1];

        const bool currIsSeparator = curr.isPunct() || curr.isSpace();
        const bool nextIsSeparator = next.isPunct() || next.isSpace();

        return (skipWordBounds || !currIsSeparator) && (nextIsSeparator);
    }

    bool isAWholeWord(const QString &text, int start, int length, bool onlyCheckBounds=false) {
        if (text.isEmpty()) { return false; }

        if (!isLeftWordBound(text, start, onlyCheckBounds)) { return false; }
        const int lastIndex = text.size() - 1;
        if (!isRightWordBound(text, lastIndex, start + length - 1, onlyCheckBounds)) { return false; }

        return true;
    }

    QString replaceWholeWords(const QString &text, const QString &replaceWhat,
                              const QString &replaceTo, Qt::CaseSensitivity caseSensitivity) {
        int pos = 0;
        const int size = replaceWhat.size();
        std::vector<std::pair<int, int> > hits;
        hits.reserve(std::max(text.length() / replaceWhat.length(), 10));

        while (pos != -1) {
            pos = text.indexOf(replaceWhat, pos, caseSensitivity);
            if (pos >= 0) {
                if (isAWholeWord(text, pos, size, true)) {
                    hits.emplace_back(std::make_pair(pos, size));
                }

                pos += size;
            }
        }

        if (hits.empty()) { return text; }

        QString result;
        result.reserve(text.length());

        int lastStart = 0;
        for (auto &hit: hits) {
            if (hit.first > lastStart) {
                result.append(text.mid(lastStart, hit.first - lastStart));
            }

            result.append(replaceTo);
            lastStart = hit.first + hit.second;
        }

        if (lastStart < text.length() - 1) {
            result.append(text.mid(lastStart));
        }

        return result;
    }

    bool containsWholeWords(const QString &haystack, const QString &needle, Qt::CaseSensitivity caseSensitivity) {
        bool anyHit = false;

        int pos = 0;
        const int size = needle.size();

        while (pos != -1) {
            pos = haystack.indexOf(needle, pos, caseSensitivity);
            if (pos >= 0) {
                if (isAWholeWord(haystack, pos, size, true)) {
                    anyHit = true;
                    break;
                }

                pos += size;
            }
        }

        return anyHit;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString getLastNLines(const QString &text, int N) {
        QString result;

        QVector<QStringRef> items = text.splitRef(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        const int length = items.length();

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
        foreachWord(text,
                    [](const QString&) { return true; },
        [&parts](int, int, const QString &word) { parts.append(word); });
    }

    std::string string_format(const std::string fmt, ...) {
        int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
        std::string str;
        va_list ap;
        while (1) {     // Maximum two passes on a POSIX system...
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
            va_end(ap);
            if (n > -1 && n < size) {  // Everything worked
                str.resize(n);
                return str;
            }
            if (n > -1)  // Needed size returned
                size = n + 1;   // For null char
            else
                size *= 2;      // Guess at a larger size (OS specific)
        }

        return str;
    }

    int levensteinDistance(const QString &s1, const QString &s2) {
        const unsigned int len1 = s1.size(), len2 = s2.size();
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

    // helper from libkexiv2
    bool isUtf8(const char * const buffer) {
        int i, n;
        register unsigned char c;
        bool gotone = false;

        if (!buffer) {
            return true;
        }

        // character never appears in text
#define F 0
        // character appears in plain ASCII text
#define T 1
        // character appears in ISO-8859 text
#define I 2
        // character appears in non-ISO extended ASCII (Mac, IBM PC)
#define X 3

        static const unsigned char text_chars[256] =
        {
            //                  BEL BS HT LF    FF CR
            F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  // 0x0X
            //                              ESC
            F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  // 0x1X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x2X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x3X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x4X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x5X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x6X
            T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  // 0x7X
            //            NEL
            X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  // 0x8X
            X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  // 0x9X
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xaX
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xbX
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xcX
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xdX
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xeX
            I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   // 0xfX
        };

        for (i = 0; (c = buffer[i]); ++i) {
            if ((c & 0x80) == 0) {
                // 0xxxxxxx is plain ASCII

                // Even if the whole file is valid UTF-8 sequences,
                // still reject it if it uses weird control characters.

                if (text_chars[c] != T) {
                    return false;
                }

            }
            else if ((c & 0x40) == 0) {
                // 10xxxxxx never 1st byte
                return false;
            }
            else {
                // 11xxxxxx begins UTF-8
                int following = 0;

                if ((c & 0x20) == 0) {
                    // 110xxxxx
                    following = 1;
                }
                else if ((c & 0x10) == 0) {
                    // 1110xxxx
                    following = 2;
                }
                else if ((c & 0x08) == 0) {
                    // 11110xxx
                    following = 3;
                }
                else if ((c & 0x04) == 0) {
                    // 111110xx
                    following = 4;
                }
                else if ((c & 0x02) == 0) {
                    // 1111110x
                    following = 5;
                }
                else {
                    return false;
                }

                for (n = 0; n < following; ++n) {
                    i++;

                    if (!(c = buffer[i])) { goto done; }

                    if ((c & 0x80) == 0 || (c & 0x40)) {
                        return false;
                    }
                }

                gotone = true;
            }
        }

done:
        return gotone;   // don't claim it's UTF-8 if it's all 7-bit.
    }

#undef F
#undef T
#undef I
#undef X

    // copy-paste code from libkexiv2
    QString detectEncodingAndDecode(const std::string &value) {
        if (value.empty()) {
            return QString();
        }

        if (isUtf8(value.c_str())) {
            return QString::fromUtf8(value.c_str());
        }

        // Utf8 has a pretty unique byte pattern.
        // Thats not true for ASCII, it is not possible
        // to reliably autodetect different ISO-8859 charsets.
        // So we can use either local encoding, or latin1.

        //TODO: KDE4PORT: check for regression of #134999 (very probably no regression!)
        return QString::fromLocal8Bit(value.c_str());
    }

    bool is7BitAscii(const QByteArray &s) {
        bool anyFault = false;
        const int size = s.size();

        for (int i = 0; i < size; i++) {
            int c = s[i];
            if (c < 0 || c >= 128) {
                anyFault = true;
                break;
            }
        }

        return !anyFault;
    }

    void extendSegmentToWordBoundaries(const QString &text, std::pair<int, int> &segment) {
        int left = segment.first;
        while (!isLeftWordBound(text, left)) { left--; }

        int right = segment.second;
        const int lastIndex = text.size() - 1;
        while (!isRightWordBound(text, lastIndex, right)) { right++; }

        segment.first = left;
        segment.second = right;
    }

    std::pair<int, int> getSegmentFromHit(const QString &text, int hit, int radius) {
        int left = hit;
        while (!isLeftWordBound(text, left)) { left--; }

        int right = hit;
        const int lastIndex = text.size() - 1;
        while (!isRightWordBound(text, lastIndex, right)) { right++; }

        int first = std::max(0, left - radius);
        int second = std::min(lastIndex, right + radius);
        return std::make_pair(first, second);
    }

    QString getUnitedHitsString(const QString &text, const std::vector<int> &hits, int radius) {
        std::vector<std::pair<int, int> > segments;
        segments.resize(hits.size());

        // create segment from each hit
        std::transform(hits.begin(), hits.end(), segments.begin(),
                       [&text, radius](int hit) {
            return getSegmentFromHit(text, hit, radius);
        });

        for (auto &item: segments) {
            extendSegmentToWordBoundaries(text, item);
        }

        auto unitedSegments = Helpers::unionRanges(segments);

        QStringList entries;
        entries.reserve((int)unitedSegments.size());

        for (auto &element: unitedSegments) {
            entries.append(text.mid(element.first, element.second - element.first + 1));
        }

        QString result = entries.join(" ... ");
        return result;
    }
}
