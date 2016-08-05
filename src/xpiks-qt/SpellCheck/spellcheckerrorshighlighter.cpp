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

#include "spellcheckerrorshighlighter.h"
#include <QColor>
#include <QSet>
#include <QString>
#include "spellcheckiteminfo.h"
#include "../QMLExtensions/colorsmodel.h"
#include "../Helpers/stringhelper.h"

namespace SpellCheck {
    SpellCheckErrorsHighlighter::SpellCheckErrorsHighlighter(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                             SpellCheckErrorsInfo *errorsInfo) :
        QSyntaxHighlighter(document),
        m_SpellCheckErrors(errorsInfo),
        m_ColorsModel(colorsModel)
    {
    }

    void SpellCheckErrorsHighlighter::highlightBlock(const QString &text) {
        if (!m_SpellCheckErrors->anyError()) { return; }

        QColor destructiveColor = m_ColorsModel->destructiveColor();

        Helpers::foreachWord(text,
                             [this](const QString &word) {
            return this->m_SpellCheckErrors->hasWrongSpelling(word); },
        [this, &destructiveColor](int start, int length, const QString &) {
            this->setFormat(start, length, destructiveColor); }
        );
    }
}
