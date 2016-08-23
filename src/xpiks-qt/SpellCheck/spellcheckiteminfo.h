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

#ifndef SPELLCHECKITEMINFO_H
#define SPELLCHECKITEMINFO_H

#include <QSet>
#include <QString>
#include <QObject>
#include <QStringList>
#include <QTextDocument>
#include <QReadWriteLock>

namespace Common {
    class BasicKeywordsModel;
}

namespace QMLExtensions {
    class ColorsModel;
}

namespace SpellCheck {
    class SpellCheckErrorsHighlighter;

    class SpellCheckErrorsInfo
    {
    public:
        bool hasWrongSpelling(const QString &word);
        void setErrorWords(const QSet<QString> &errors);
        bool removeWordFromSet(const QString &word);
        bool anyError();
        void clear();
        QStringList toList();

    private:
        QSet<QString> m_WordsWithErrors;
        QReadWriteLock m_ErrorsLock;
    };

    class SpellCheckItemInfo
    {
    public:
        void setDescriptionErrors(const QSet<QString> &errors);
        void setTitleErrors(const QSet<QString> &errors);
        void removeWordsFromErrors(const QStringList &words);
        void createHighlighterForDescription(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                             Common::BasicKeywordsModel *basicKeywordsModel);
        void createHighlighterForTitle(QTextDocument *document, QMLExtensions::ColorsModel *colorsModel,
                                       Common::BasicKeywordsModel *basicKeywordsModel);

        bool hasDescriptionError(const QString &word) { return m_DescriptionErrors.hasWrongSpelling(word); }
        bool hasTitleError(const QString &word) { return m_TitleErrors.hasWrongSpelling(word); }
        void clear() { m_DescriptionErrors.clear(); m_TitleErrors.clear(); }

    private:
        SpellCheckErrorsInfo m_DescriptionErrors;
        SpellCheckErrorsInfo m_TitleErrors;
    };
}

#endif // SPELLCHECKITEMINFO_H
