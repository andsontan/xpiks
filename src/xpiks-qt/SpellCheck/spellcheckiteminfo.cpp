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

#include "spellcheckiteminfo.h"
#include "spellcheckerrorshighlighter.h"
#include "../Common/basickeywordsmodel.h"
#include "../QMLExtensions/colorsmodel.h"

namespace SpellCheck {
    void SpellCheckItemInfo::setDescriptionErrors(const QSet<QString> &errors) {
        m_DescriptionErrors.setErrorWords(errors);
    }

    void SpellCheckItemInfo::setTitleErrors(const QSet<QString> &errors) {
        m_TitleErrors.setErrorWords(errors);
    }

    void SpellCheckItemInfo::createHighlighterForDescription(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                             Common::BasicKeywordsModel *basicKeywordsModel) {
        // is freed by the document
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_DescriptionErrors);
        QObject::connect(basicKeywordsModel, SIGNAL(spellCheckResultsReady()),
                         highlighter, SLOT(rehighlight()));
        QObject::connect(colorsModel, SIGNAL(themeChanged()),
                         highlighter, SLOT(rehighlight()));
    }

    void SpellCheckItemInfo::createHighlighterForTitle(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                                       Common::BasicKeywordsModel *basicKeywordsModel) {
        // is freed by the document
        SpellCheckErrorsHighlighter *highlighter = new SpellCheckErrorsHighlighter(document, colorsModel, &m_TitleErrors);
        QObject::connect(basicKeywordsModel, SIGNAL(spellCheckResultsReady()),
                         highlighter, SLOT(rehighlight()));
        QObject::connect(colorsModel, SIGNAL(themeChanged()),
                         highlighter, SLOT(rehighlight()));
    }
}

