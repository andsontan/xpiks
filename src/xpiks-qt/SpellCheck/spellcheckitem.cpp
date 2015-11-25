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

#include "spellcheckitem.h"
#include "../Models/artworkmetadata.h"
#include <QStringList>
#include "../Helpers/indiceshelper.h"
#include "ispellcheckable.h"

namespace SpellCheck {
    SpellCheckItemBase::~SpellCheckItemBase() {
        qDeleteAll(m_QueryItems);
    }

    void SpellCheckItemBase::accountResultAt(int index) {
        if (0 <= index && index < m_QueryItems.length()) {
            SpellCheckQueryItem *item = m_QueryItems[index];
            m_SpellCheckResults[item->m_Word] = item->m_IsCorrect;
        }
    }

    bool SpellCheckItemBase::getIsCorrect(const QString &word) const {
        bool result = m_SpellCheckResults.value(word, true);
        return result;
    }

    void SpellCheckItemBase::appendItem(SpellCheckQueryItem *item) {
        m_QueryItems.append(item);
    }

    SpellCheckItem::SpellCheckItem(ISpellCheckable *spellCheckable, int keywordIndex) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable)
    {
        QString keyword = m_SpellCheckable->retrieveKeyword(keywordIndex);
        SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(keywordIndex, keyword);
        appendItem(queryItem);
    }

    SpellCheckItem::SpellCheckItem(ISpellCheckable *spellCheckable) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable)
    {
        QStringList keywords = spellCheckable->getKeywords();
        addWords(keywords, 0);

        QStringList descriptionWords = spellCheckable->getDescriptionWords();
        addWords(descriptionWords, keywords.length() * 1000);

        QStringList titleWords = spellCheckable->getTitleWords();
        addWords(titleWords, keywords.length() * 1000);
    }

    void SpellCheckItem::addWords(const QStringList &words, int startingIndex) {
        int index = startingIndex;

        foreach (const QString &word, words) {
            SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(index, word);
            appendItem(queryItem);
            index++;
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const QList<SpellCheckQueryItem*> &items = getQueries();
        m_SpellCheckable->setSpellCheckResults(items);
        m_SpellCheckable->setSpellCheckResults(getHash());
        int index = items.length() == 1 ? items.first()->m_Index : -1;
        emit resultsReady(index);
    }
}
