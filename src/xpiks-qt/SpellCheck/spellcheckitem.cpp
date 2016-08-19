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
#include "../Common/basickeywordsmodel.h"

namespace SpellCheck {
    SpellCheckItemBase::~SpellCheckItemBase() {
    }

    void SpellCheckItemBase::accountResultAt(int index) {
        if (0 <= index && (size_t)index < m_QueryItems.size()) {
            auto &item = m_QueryItems.at(index);
            m_SpellCheckResults[item->m_Word] = item->m_IsCorrect;
        }
    }

    bool SpellCheckItemBase::getIsCorrect(const QString &word) const {
        bool result = m_SpellCheckResults.value(word, true);
        return result;
    }

    void SpellCheckItemBase::appendItem(const std::shared_ptr<SpellCheckQueryItem> &item) {
        m_QueryItems.push_back(item);
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, int spellCheckFlags, int keywordIndex) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(true)
    {
        Q_ASSERT(Common::HasFlag(spellCheckFlags, Common::SpellCheckKeywords));
        Q_ASSERT(spellCheckable != NULL);

        spellCheckable->acquire();

        QString keyword = m_SpellCheckable->retrieveKeyword(keywordIndex);
        if (!keyword.contains(QChar::Space)) {
            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, keyword));
            appendItem(queryItem);
        } else {
            QStringList parts = keyword.split(QChar::Space, QString::SkipEmptyParts);
            foreach (const QString &part, parts) {
                QString item = part.trimmed();
                std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, item));
                appendItem(queryItem);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, int spellCheckFlags) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(false)
    {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> alwaysTrue = [](const QString &) {return true;};

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckKeywords)) {
            QStringList keywords = spellCheckable->getKeywords();
            reserve(keywords.length());
            addWords(keywords, 0, alwaysTrue);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckDescription)) {
            QStringList descriptionWords = spellCheckable->getDescriptionWords();
            reserve(descriptionWords.length());
            addWords(descriptionWords, 100000, alwaysTrue);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckTitle)) {
            QStringList titleWords = spellCheckable->getTitleWords();
            reserve(titleWords.length());
            addWords(titleWords, 100000, alwaysTrue);
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, const QString &keywordToCheck) :
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(Common::SpellCheckAll),
        m_OnlyOneKeyword(false)
    {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> sameKeywordFunc = [&keywordToCheck](const QString &word) {
            return QString::compare(word, keywordToCheck, Qt::CaseInsensitive) == 0;
        };

        QStringList keywords = spellCheckable->getKeywords();
        reserve(keywords.length());
        addWords(keywords, 0, sameKeywordFunc);

        QStringList descriptionWords = spellCheckable->getDescriptionWords();
        reserve(descriptionWords.length());
        addWords(descriptionWords, 100000, sameKeywordFunc);

        std::function<bool (const QString &word)> containsFunc = [&keywordToCheck](const QString &word) {
            return word.contains(keywordToCheck, Qt::CaseInsensitive);
        };

        QStringList titleWords = spellCheckable->getTitleWords();
        reserve(titleWords.length());
        addWords(titleWords, 100000, containsFunc);
    }

    SpellCheckItem::~SpellCheckItem() {
        if (m_SpellCheckable->release()) {
            LOG_WARNING << "Item could have been removed";
        }
    }

    void SpellCheckItem::addWords(const QStringList &words, int startingIndex, const std::function<bool (const QString &word)> &pred) {
        int index = startingIndex;

        foreach (const QString &word, words) {
            bool added = false;

            if (!word.contains(QChar::Space)) {
                if (pred(word)) {
                    std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, word));
                    appendItem(queryItem);
                    added = true;
                }
            } else {
                QStringList parts = word.split(QChar::Space, QString::SkipEmptyParts);
                foreach (const QString &part, parts) {
                    QString item = part.trimmed();
                    if (item.length() >= 2) {
                        if (pred(item)) {
                            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, item));
                            appendItem(queryItem);
                            added = true;
                        }
                    }
                }
            }

            if (added) {
                index++;
            }
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const std::vector<std::shared_ptr<SpellCheckQueryItem> > &items = getQueries();

        // can be empty in case of clear command
        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckKeywords) && !items.empty()) {
            m_SpellCheckable->setSpellCheckResults(items, m_OnlyOneKeyword);
        }

        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckDescription) ||
                Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckTitle)) {
            m_SpellCheckable->setSpellCheckResults(getHash(), m_SpellCheckFlags);
        }

        int index = m_OnlyOneKeyword ? items.front()->m_Index : -1;
        emit resultsReady(m_SpellCheckFlags, index);
    }
}
