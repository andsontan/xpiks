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

#ifndef SPELLCHECKITEMINFO_H
#define SPELLCHECKITEMINFO_H

#include <QSet>
#include <QString>
#include <QTextDocument>

namespace Models {
    class ArtworkMetadata;
}

namespace SpellCheck {
    class SpellCheckErrorsHighlighter;

    class SpellCheckErrorsInfo {
    public:
        bool hasWrongSpelling(const QString& word) const { return m_WordsWithErrors.contains(word); }
        void setErrorWords(const QSet<QString> &errors) { m_WordsWithErrors.clear(); m_WordsWithErrors.unite(errors); }
        bool anyError() const { return !m_WordsWithErrors.isEmpty(); }

    private:
        QSet<QString> m_WordsWithErrors;
    };

    class SpellCheckItemInfo
    {
    public:
        bool anyDescriptionError() const { return m_DescriptionErrors.anyError(); }
        bool anyTitleError() const { return m_TitleErrors.anyError(); }
        void setDescriptionErrors(const QSet<QString> &errors);
        void setTitleErrors(const QSet<QString> &errors);
        void createHighlighterForDescription(QTextDocument *document, Models::ArtworkMetadata *metadata);
        void createHighlighterForTitle(QTextDocument *document, Models::ArtworkMetadata *metadata);

    private:
        SpellCheckErrorsInfo m_DescriptionErrors;
        SpellCheckErrorsInfo m_TitleErrors;
    };
}

#endif // SPELLCHECKITEMINFO_H
