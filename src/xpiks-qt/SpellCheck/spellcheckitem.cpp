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
        int index = 0;

        foreach (const QString &word, keywords) {
            SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(index, word);
            appendItem(queryItem);
            index++;
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const QList<SpellCheckQueryItem*> &items = getQueries();
        m_SpellCheckable->setSpellCheckResults(items);
        int index = items.length() == 1 ? items.first()->m_Index : -1;
        emit resultsReady(index);
    }
}
