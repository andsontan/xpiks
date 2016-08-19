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

#ifndef ISPELLCHECKABLE
#define ISPELLCHECKABLE

#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <QPair>
#include "spellcheckitem.h"
#include "spellsuggestionsitem.h"
#include "../Common/ibasicartwork.h"
#include "../Common/flags.h"

namespace SpellCheck {
    class KeywordSpellSuggestions;

    class ISpellCheckable {
    public:
        virtual ~ISpellCheckable() {}

        virtual QString retrieveKeyword(int wordIndex) = 0;
        virtual void setSpellCheckResults(const std::vector<std::shared_ptr<SpellCheckQueryItem> > &items) = 0;
        virtual void setSpellCheckResults(const QHash<QString, bool> &results, int flags) = 0;
        virtual std::vector<std::shared_ptr<SpellSuggestionsItem> > createKeywordsSuggestionsList() = 0;
        virtual std::vector<std::shared_ptr<SpellSuggestionsItem> > createDescriptionSuggestionsList() = 0;
        virtual std::vector<std::shared_ptr<SpellSuggestionsItem> > createTitleSuggestionsList() = 0;
        virtual Common::KeywordReplaceResult fixKeywordSpelling(int index, const QString &existing, const QString &replacement) = 0;
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) = 0;
        virtual void fixDescriptionSpelling(const QString &word, const QString &replacement) = 0;
        virtual void fixTitleSpelling(const QString &word, const QString &replacement) = 0;
        virtual void afterReplaceCallback() = 0;
        virtual void connectSignals(SpellCheckItem *item) = 0;
        virtual QStringList getDescriptionWords() = 0;
        virtual QStringList getTitleWords() = 0;
    };
}

#endif // ISPELLCHECKABLE

