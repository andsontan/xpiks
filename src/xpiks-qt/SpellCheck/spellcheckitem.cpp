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

QStringList simplifyList(const QStringList &items) {
    QStringList processed;
    processed.reserve(items.length());

    foreach (const QString &item, items) {
        if (item.contains(QChar::Space)) {
            processed.append(item.split(QChar::Space, QString::SkipEmptyParts));
        } else {
            processed.append(item);
        }
    }

    return processed;
}

namespace SpellCheck {
    SpellCheckItemBase::~SpellCheckItemBase() {
        qDeleteAll(m_QueryItems);
    }

    void SpellCheckItemBase::accountResultAt(int index) {
        if (0 <= index && index < m_QueryItems.length()) {
            SpellCheckQueryItem *item = m_QueryItems.at(index);
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
        m_SpellCheckable(spellCheckable),
        m_OnlyOneKeyword(true)
    {
        QString keyword = m_SpellCheckable->retrieveKeyword(keywordIndex);
        if (!keyword.contains(QChar::Space)) {
            SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(keywordIndex, keyword);
            appendItem(queryItem);
        } else {
            QStringList parts = keyword.split(QChar::Space, QString::SkipEmptyParts);
            foreach (const QString &part, parts) {
                QString item = part.trimmed();
                SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(keywordIndex, item);
                appendItem(queryItem);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(ISpellCheckable *spellCheckable) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_OnlyOneKeyword(false)
    {
        QStringList keywords = simplifyList(spellCheckable->getKeywords());
        QStringList descriptionWords = spellCheckable->getDescriptionWords();
        QStringList titleWords = spellCheckable->getTitleWords();

        reserve(keywords.length() + descriptionWords.length() + titleWords.length());

        addWords(keywords, 0);
        addWords(descriptionWords, keywords.length() * 1000);
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
        const QVector<SpellCheckQueryItem*> &items = getQueries();
        m_SpellCheckable->setSpellCheckResults(items, m_OnlyOneKeyword);
        m_SpellCheckable->setSpellCheckResults(getHash());
        int index = m_OnlyOneKeyword ? items.first()->m_Index : -1;
        emit resultsReady(index);
    }
}
