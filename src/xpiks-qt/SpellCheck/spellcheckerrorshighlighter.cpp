/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#include "spellcheckerrorshighlighter.h"
#include <QColor>
#include <QSet>
#include <QString>

namespace SpellCheck {
    SpellCheckErrorsHighlighter::SpellCheckErrorsHighlighter(QTextDocument *document) :
        QSyntaxHighlighter(document)
    {
    }

    void SpellCheckErrorsHighlighter::setErrorWords(const QSet<QString> &items) {
        m_WordsWithErrors.clear();
        m_WordsWithErrors.unite(items);
    }

    void SpellCheckErrorsHighlighter::highlightBlock(const QString &text) {
        int i = 0;
        int size = text.size();
        int lastStart = -1;

        // destructiveColor from Colors.js
        QColor destructiveColor = QColor::fromRgb(209, 11, 11);

        while (i < size) {
            QChar c = text[i];
            if (c == QChar::Space || c == QChar::Tabulation || c == QChar::CarriageReturn) {
                if (lastStart != -1) {
                    int wordLength = i - lastStart;
                    if (hasWrongSpelling(text, lastStart, wordLength)) {
                        setFormat(lastStart, wordLength, destructiveColor);
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
            if (hasWrongSpelling(text, lastStart, wordLength)) {
                setFormat(lastStart, wordLength, destructiveColor);
            }
        }
    }

    bool SpellCheckErrorsHighlighter::hasWrongSpelling(const QString &text, int start, int count) const {
        QString substring = text.mid(start, count);
        bool isWrong = m_WordsWithErrors.contains(substring);
        return isWrong;
    }
}
