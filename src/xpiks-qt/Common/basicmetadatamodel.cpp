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

#include "basicmetadatamodel.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordshelpers.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"

namespace Common {
    BasicMetadataModel::BasicMetadataModel(Hold &hold, QObject *parent):
        BasicKeywordsModel(hold, parent),
        m_SpellCheckInfo(NULL)
    { }

    QString BasicMetadataModel::getDescription() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description;
    }

    QString BasicMetadataModel::getTitle() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title;
    }

#ifdef CORE_TESTS
    void BasicMetadataModel::initialize(const QString &title, const QString &description, const QString &rawKeywords) {
        setTitle(title);
        setDescription(description);
        setKeywords(rawKeywords.split(',', QString::SkipEmptyParts));
    }
#endif

    void BasicMetadataModel::setSpellCheckResults(const QHash<QString, bool> &results, Common::SpellCheckFlags flags) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Description)) {
            updateDescriptionSpellErrors(results);
        }

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Title)) {
            updateTitleSpellErrors(results);
        }
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > BasicMetadataModel::createDescriptionSuggestionsList() {
        LOG_DEBUG << "#";
        QStringList descriptionWords = getDescriptionWords();
        int length = descriptionWords.length();

        std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = descriptionWords.at(i);
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                LOG_DEBUG << word << "has wrong spelling";

                spellCheckSuggestions.emplace_back(new SpellCheck::DescriptionSpellSuggestions(word));
            }
        }

        return spellCheckSuggestions;
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > BasicMetadataModel::createTitleSuggestionsList() {
        LOG_DEBUG << "#";
        QStringList titleWords = getTitleWords();
        int length = titleWords.length();

        std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = titleWords.at(i);
            if (m_SpellCheckInfo->hasTitleError(word)) {
                LOG_DEBUG << word << "has wrong spelling";

                spellCheckSuggestions.emplace_back(new SpellCheck::TitleSpellSuggestions(word));
            }
        }

        return spellCheckSuggestions;
    }

    void BasicMetadataModel::fixDescriptionSpelling(const QString &word, const QString &replacement) {
        Common::SearchFlags flags = Common::SearchFlags::None;
        Common::SetFlag(flags, Common::SearchFlags::CaseSensitive);
        Common::SetFlag(flags, Common::SearchFlags::Description);

        replaceInDescription(word, replacement, flags);
    }

    void BasicMetadataModel::fixTitleSpelling(const QString &word, const QString &replacement) {
        Common::SearchFlags flags = Common::SearchFlags::None;
        Common::SetFlag(flags, Common::SearchFlags::CaseSensitive);
        Common::SetFlag(flags, Common::SearchFlags::Description);

        replaceInTitle(word, replacement, flags);
    }

    QStringList BasicMetadataModel::getDescriptionWords() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Description, words);
        return words;
    }

    QStringList BasicMetadataModel::getTitleWords() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Title, words);
        return words;
    }

    bool BasicMetadataModel::replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                                  Common::SearchFlags flags) {
        LOG_DEBUG << "#";
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::ExactMatch);
        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QString description = getDescription();
        if (!wholeWords) {
            description.replace(replaceWhat, replaceTo, caseSensivity);
        } else {
            description = Helpers::replaceWholeWords(description, replaceWhat, replaceTo, caseSensivity);
        }

        bool result = setDescription(description);
        return result;
    }

    bool BasicMetadataModel::replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                                            Common::SearchFlags flags) {
        LOG_DEBUG << "#";
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::ExactMatch);
        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QString title = getTitle();
        if (!wholeWords) {
            title.replace(replaceWhat, replaceTo, caseSensivity);
        } else {
            title = Helpers::replaceWholeWords(title, replaceWhat, replaceTo, caseSensivity);
        }

        bool result = setTitle(title);
        return result;
    }

    bool BasicMetadataModel::replace(const QString &replaceWhat, const QString &replaceTo, SearchFlags flags) {
        LOG_INFO << replaceWhat << "->" << replaceTo << "with flags:" << (int)flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(!replaceTo.isEmpty());
        Q_ASSERT(((int)flags & (int)Common::SearchFlags::Metadata) != 0);
        bool anyChanged = false;

        anyChanged = BasicKeywordsModel::replace(replaceWhat, replaceTo, flags);

        const bool needToCheckDescription = Common::HasFlag(flags, Common::SearchFlags::Description);
        if (needToCheckDescription) {
            if (this->replaceInDescription(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        const bool needToCheckTitle = Common::HasFlag(flags, Common::SearchFlags::Title);
        if (needToCheckTitle) {
            if (this->replaceInTitle(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        return anyChanged;
    }

    bool BasicMetadataModel::hasDescriptionSpellError() {
        bool anyError = false;

        const QStringList &descriptionWords = getDescriptionWords();

        foreach(const QString &word, descriptionWords) {
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicMetadataModel::hasTitleSpellError() {
        bool anyError = false;

        const QStringList &titleWords = getTitleWords();

        foreach(const QString &word, titleWords) {
            if (m_SpellCheckInfo->hasTitleError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicMetadataModel::hasDescriptionWordSpellError(const QString &word) {
        return m_SpellCheckInfo->hasDescriptionError(word);
    }

    bool BasicMetadataModel::hasTitleWordSpellError(const QString &word) {
        return m_SpellCheckInfo->hasTitleError(word);
    }

    bool BasicMetadataModel::hasSpellErrors() {
        bool result = hasDescriptionSpellError() ||
                hasTitleSpellError() ||
                BasicKeywordsModel::hasSpellErrors();

        return result;
    }

    bool BasicMetadataModel::setDescription(const QString &value) {
        QWriteLocker writeLocker(&m_DescriptionLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Description;
        if (result) {
            m_Description = value;
        }

        return result;
    }

    bool BasicMetadataModel::setTitle(const QString &value) {
        QWriteLocker writeLocker(&m_TitleLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Title;
        if (result) {
            m_Title = value;
        }

        return result;
    }

    bool BasicMetadataModel::isEmpty() {
        bool isEmpty = BasicKeywordsModel::isEmpty();

        if (!isEmpty) {
            QReadLocker readDescriptionLock(&m_DescriptionLock);
            Q_UNUSED(readDescriptionLock);
            isEmpty = m_Description.trimmed().isEmpty();
        }

        return isEmpty;
    }

    bool BasicMetadataModel::isTitleEmpty() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title.trimmed().isEmpty();
    }

    bool BasicMetadataModel::isDescriptionEmpty() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description.trimmed().isEmpty();
    }

    void BasicMetadataModel::clearModel() {
        setDescription("");
        setTitle("");
        clearKeywords();
    }

    void BasicMetadataModel::notifyDescriptionSpellCheck() {
        emit spellCheckResultsReady();
    }

    void BasicMetadataModel::notifyTitleSpellCheck() {
        emit spellCheckResultsReady();
    }

    void BasicMetadataModel::updateDescriptionSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> descriptionErrors;
        QStringList descriptionWords = getDescriptionWords();
        foreach(const QString &word, descriptionWords) {
            if (results.value(word, true) == false) {
                descriptionErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setDescriptionErrors(descriptionErrors);
    }

    void BasicMetadataModel::updateTitleSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> titleErrors;
        QStringList titleWords = getTitleWords();
        foreach(const QString &word, titleWords) {
            if (results.value(word, true) == false) {
                titleErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setTitleErrors(titleErrors);
    }
}
