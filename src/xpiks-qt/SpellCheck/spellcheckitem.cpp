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

#include "spellcheckitem.h"
#include "../Models/artworkmetadata.h"
#include <QStringList>
#include "../Helpers/indiceshelper.h"
#include "ispellcheckable.h"
#include "../Common/flags.h"
#include "../Common/defines.h"

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

    SpellCheckItem::SpellCheckItem(ISpellCheckable *spellCheckable, int spellCheckFlags, int keywordIndex) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(true)
    {
        Q_ASSERT(Common::HasFlag(spellCheckFlags, Common::SpellCheckKeywords));

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

    SpellCheckItem::SpellCheckItem(ISpellCheckable *spellCheckable, int spellCheckFlags) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(false)
    {
        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckKeywords)) {
            QStringList keywords = spellCheckable->getKeywords();
            reserve(keywords.length());
            addWords(keywords, 0);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckDescription)) {
            QStringList descriptionWords = spellCheckable->getDescriptionWords();
            reserve(descriptionWords.length());
            addWords(descriptionWords, 100000);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckTitle)) {
            QStringList titleWords = spellCheckable->getTitleWords();
            reserve(titleWords.length());
            addWords(titleWords, 100000);
        }
    }

    void SpellCheckItem::addWords(const QStringList &words, int startingIndex) {
        int index = startingIndex;

        foreach (const QString &word, words) {
            if (!word.contains(QChar::Space)) {
                SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(index, word);
                appendItem(queryItem);
            } else {
                QStringList parts = word.split(QChar::Space, QString::SkipEmptyParts);
                foreach (const QString &part, parts) {
                    QString item = part.trimmed();
                    SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(index, item);
                    appendItem(queryItem);
                }
            }

            index++;
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const QVector<SpellCheckQueryItem*> &items = getQueries();

        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckKeywords)) {
            m_SpellCheckable->setSpellCheckResults(items, m_OnlyOneKeyword);
        }

        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckDescription) ||
                Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckTitle)) {
            m_SpellCheckable->setSpellCheckResults(getHash(), m_SpellCheckFlags);
        }

        int index = m_OnlyOneKeyword ? items.first()->m_Index : -1;
        emit resultsReady(m_SpellCheckFlags, index);

        bool releaseResult = m_SpellCheckable->release();
        if (releaseResult) {
            LOG_WARNING << "item was locked for removal";
        }
    }
}
