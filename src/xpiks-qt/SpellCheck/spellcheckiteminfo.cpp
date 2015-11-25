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

#include "spellcheckiteminfo.h"
#include <QTextDocument>
#include "spellcheckerrorshighlighter.h"
#include "../Models/artworkmetadata.h"

namespace SpellCheck {
    SpellCheckItemInfo::SpellCheckItemInfo():
        m_DescriptionErrorsHighlighter(NULL),
        m_TitleErrorsHighlighter(NULL)
    {
    }

    void SpellCheckItemInfo::setDescriptionErrors(const QSet<QString> &errors) {
        m_ErrorsInDescription = errors;
        m_DescriptionErrorsHighlighter->setErrorWords(errors);
    }

    void SpellCheckItemInfo::setTitleErrors(const QSet<QString> &errors) {
        m_ErrorsInTitle = errors;
        m_TitleErrorsHighlighter->setErrorWords(errors);
    }

    void SpellCheckItemInfo::createHighlighterForDescription(QTextDocument *document,
                                                             Models::ArtworkMetadata *metadata) {
        if (m_DescriptionErrorsHighlighter == NULL) {
            m_DescriptionErrorsHighlighter = new SpellCheckErrorsHighlighter(document);
            m_DescriptionErrorsHighlighter->setErrorWords(m_ErrorsInDescription);

            QObject::connect(metadata, SIGNAL(spellCheckResultsReady()),
                             m_DescriptionErrorsHighlighter, SLOT(rehighlight()));
        }
    }

    void SpellCheckItemInfo::createHighlighterForTitle(QTextDocument *document,
                                                       Models::ArtworkMetadata *metadata) {
        if (m_TitleErrorsHighlighter == NULL) {
            m_TitleErrorsHighlighter = new SpellCheckErrorsHighlighter(document);
            m_TitleErrorsHighlighter->setErrorWords(m_ErrorsInTitle);

            QObject::connect(metadata, SIGNAL(spellCheckResultsReady()),
                             m_TitleErrorsHighlighter, SLOT(rehighlight()));
        }
    }
}

