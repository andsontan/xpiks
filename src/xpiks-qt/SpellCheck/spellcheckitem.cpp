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
#include "../Common/flags.h"
#include "../Common/defines.h"
#include "../Common/basicmetadatamodel.h"
#include "../Helpers/stringhelper.h"

namespace SpellCheck {
    SpellCheckItemBase::~SpellCheckItemBase() {}

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

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags, int keywordIndex):
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(true) {
        Q_ASSERT(Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Keywords));
        Q_ASSERT(spellCheckable != NULL);

        spellCheckable->acquire();

        QString keyword = m_SpellCheckable->retrieveKeyword(keywordIndex);
        if (!keyword.contains(QChar::Space)) {
            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, keyword));
            appendItem(queryItem);
        } else {
            QStringList parts = keyword.split(QChar::Space, QString::SkipEmptyParts);
            foreach(const QString &part, parts) {
                QString item = part.trimmed();
                std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(keywordIndex, item));

                appendItem(queryItem);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags):
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(spellCheckFlags),
        m_OnlyOneKeyword(false) {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> alwaysTrue = [](const QString &) {return true; };

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Keywords)) {
            QStringList keywords = spellCheckable->getKeywords();
            reserve(keywords.length());
            addWords(keywords, 0, alwaysTrue);
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Description)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
            if (metadataModel != nullptr) {
                QStringList descriptionWords = metadataModel->getDescriptionWords();
                reserve(descriptionWords.length());
                addWords(descriptionWords, 100000, alwaysTrue);
            }
        }

        if (Common::HasFlag(spellCheckFlags, Common::SpellCheckFlags::Title)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
            if (metadataModel != nullptr) {
                QStringList titleWords = metadataModel->getTitleWords();
                reserve(titleWords.length());
                addWords(titleWords, 100000, alwaysTrue);
            }
        }
    }

    SpellCheckItem::SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, const QStringList &keywordsToCheck):
        SpellCheckItemBase(),
        m_SpellCheckable(spellCheckable),
        m_SpellCheckFlags(Common::SpellCheckFlags::All),
        m_OnlyOneKeyword(false)
    {
        Q_ASSERT(spellCheckable != NULL);
        spellCheckable->acquire();

        std::function<bool (const QString &word)> containsFunc = [&keywordsToCheck](const QString &word) {
                                                                     bool contains = false;

                                                                     for (auto &item: keywordsToCheck) {
                                                                         if (word.contains(item, Qt::CaseInsensitive)) {
                                                                             contains = true;
                                                                             break;
                                                                         }
                                                                     }

                                                                     return contains;
                                                                 };

        QStringList keywords = spellCheckable->getKeywords();
        reserve(keywords.length());
        addWords(keywords, 0, containsFunc);

        std::function<bool (const QString &word)> sameKeywordFunc = [&keywordsToCheck](const QString &word) {
                                                                        bool match = false;

                                                                        for (auto &item: keywordsToCheck) {
                                                                            if (QString::compare(word, item, Qt::CaseInsensitive) == 0) {
                                                                                match = true;
                                                                                break;
                                                                            }
                                                                        }

                                                                        return match;
                                                                    };

        Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(spellCheckable);
        if (metadataModel != nullptr) {
            QStringList descriptionWords = metadataModel->getDescriptionWords();
            reserve(descriptionWords.length());
            addWords(descriptionWords, 100000, sameKeywordFunc);

            QStringList titleWords = metadataModel->getTitleWords();
            reserve(titleWords.length());
            addWords(titleWords, 100000, sameKeywordFunc);
        }
    }

    SpellCheckItem::~SpellCheckItem() {
        if (m_SpellCheckable->release()) {
            LOG_WARNING << "Item could have been removed";
        }
    }

    void SpellCheckItem::addWords(const QStringList &words, int startingIndex, const std::function<bool (const QString &word)> &pred) {
        int index = startingIndex;

        foreach(const QString &word, words) {
            if (!word.contains(QChar::Space)) {
                if (pred(word)) {
                    std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, word));
                    appendItem(queryItem);
                }
            } else {
                QStringList parts = word.split(QChar::Space, QString::SkipEmptyParts);
                foreach(const QString &part, parts) {
                    QString item = part.trimmed();

                    if (item.length() >= 2) {
                        if (pred(item)) {
                            std::shared_ptr<SpellCheckQueryItem> queryItem(new SpellCheckQueryItem(index, item));
                            appendItem(queryItem);
                        }
                    }
                }
            }

            index++;
        }
    }

    /*virtual */
    void SpellCheckItem::submitSpellCheckResult() {
        const std::vector<std::shared_ptr<SpellCheckQueryItem> > &items = getQueries();

        // can be empty in case of clear command
        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Keywords) && !items.empty()) {
            m_SpellCheckable->setKeywordsSpellCheckResults(items);
        }

        if (Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Description) ||
            Common::HasFlag(m_SpellCheckFlags, Common::SpellCheckFlags::Title)) {
            Common::BasicMetadataModel *metadataModel = dynamic_cast<Common::BasicMetadataModel*>(m_SpellCheckable);
            if (metadataModel != nullptr) {
                metadataModel->setSpellCheckResults(getHash(), m_SpellCheckFlags);
            }
        }

        int index = m_OnlyOneKeyword ? items.front()->m_Index : -1;
        emit resultsReady(m_SpellCheckFlags, index);
    }

    AddWordToUserDictItem::AddWordToUserDictItem(const QString &keyword):
        m_ClearFlag(false)
    {
        Helpers::splitText(keyword, m_KeywordsToAdd);
        m_KeywordsToAdd.removeDuplicates();
    }

    AddWordToUserDictItem::AddWordToUserDictItem(bool clearFlag):
        m_ClearFlag(clearFlag)
    { }
}
