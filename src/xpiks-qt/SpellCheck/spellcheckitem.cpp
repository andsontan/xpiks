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

namespace SpellCheck {
    SpellCheckItem::SpellCheckItem(Models::ArtworkMetadata *metadata, int keywordIndex) :
        m_Metadata(metadata)
    {
        QString keyword = metadata->retrieveKeyword(keywordIndex);
        SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(keywordIndex, keyword);
        m_QueryItems.append(queryItem);
    }

    SpellCheckItem::SpellCheckItem(Models::ArtworkMetadata *metadata) {
        QStringList keywords = metadata->getKeywords();
        int index = 0;

        foreach (const QString &word, keywords) {
            SpellCheckQueryItem *queryItem = new SpellCheckQueryItem(index, word);
            m_QueryItems.append(queryItem);
            index++;
        }
    }

    SpellCheckItem::~SpellCheckItem() {
        qDeleteAll(m_QueryItems);
    }

    void SpellCheckItem::submitSpellCheckResult() const {
        m_Metadata->lockRead();
        {
            foreach (SpellCheckQueryItem *item, m_QueryItems) {
                m_Metadata->setSpellCheckResultUnsafe(item->m_CheckResult, item->m_Index, item->m_Keyword);
            }
        }
        m_Metadata->unlock();

        int index = -1;

        if (m_QueryItems.length() == 1) {
            index = m_QueryItems.first()->m_Index;
        }

        m_Metadata->emitSpellCheckChanged(index);
    }
}
