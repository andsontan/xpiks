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

#include "artworkproxybase.h"
#include "../Commands/commandmanager.h"

namespace Models {
    QString ArtworkProxyBase::getDescription() {
        auto *basicModel = getBasicMetadataModel();
        return basicModel->getDescription();
    }

    QString ArtworkProxyBase::getTitle() {
        auto *basicModel = getBasicMetadataModel();
        return basicModel->getTitle();
    }

    int ArtworkProxyBase::getKeywordsCount() {
        auto *basicModel = getBasicMetadataModel();
        return basicModel->getKeywordsCount();
    }

    void ArtworkProxyBase::setDescription(const QString &description) {
        if (doSetDescription(description)) {
            signalDescriptionChanged();
        }
    }

    void ArtworkProxyBase::setTitle(const QString &title) {
        if (doSetTitle(title)) {
            signalTitleChanged();
        }
    }

    bool ArtworkProxyBase::doSetDescription(const QString &description) {
        auto *metadataOperator = getMetadataOperator();
        return metadataOperator->setDescription(description);
    }

    bool ArtworkProxyBase::doSetTitle(const QString &title) {
        auto *metadataOperator = getMetadataOperator();
        return metadataOperator->setTitle(title);
    }

    bool ArtworkProxyBase::doEditKeyword(int index, const QString &replacement) {
        LOG_INFO << "index:" << index << "replacement:" << replacement;
        auto *metadataOperator = getMetadataOperator();
        bool result = metadataOperator->editKeyword(index, replacement);
        if (result) {
            auto *basicModel = getBasicMetadataModel();
            m_CommandManager->submitKeywordForSpellCheck(basicModel, index);
        } else {
            LOG_INFO << "Failed to edit to" << replacement;
        }

        return result;
    }

    bool ArtworkProxyBase::doRemoveKeywordAt(int keywordIndex, QString &keyword) {
        LOG_INFO << "keyword index:" << keywordIndex;
        auto *metadataOperator = getMetadataOperator();
        bool result = metadataOperator->removeKeywordAt(keywordIndex, keyword);
        if (result) {
            signalKeywordsCountChanged();
            LOG_INFO << "Removed keyword:" << keyword << "keywords count:" << getKeywordsCount();
        }

        return result;
    }

    bool ArtworkProxyBase::doRemoveLastKeyword(QString &keyword) {
        LOG_DEBUG << "#";
        auto *metadataOperator = getMetadataOperator();
        bool result = metadataOperator->removeLastKeyword(keyword);
        if (result) {
            signalKeywordsCountChanged();
            LOG_INFO << "Removed keyword:" << keyword << "keywords count:" << getKeywordsCount();
        }

        return result;
    }

    bool ArtworkProxyBase::doAppendKeyword(const QString &keyword) {
        LOG_INFO << keyword;
        auto *metadataOperator = getMetadataOperator();
        bool result = metadataOperator->appendKeyword(keyword);
        if (result) {
            signalKeywordsCountChanged();
            auto *basicModel = getBasicMetadataModel();
            m_CommandManager->submitKeywordForSpellCheck(basicModel, basicModel->rowCount() - 1);
        } else {
            LOG_INFO << "Failed to append:" << keyword;
        }

        return result;
    }

    int ArtworkProxyBase::doAppendKeywords(const QStringList &keywords) {
        LOG_INFO << keywords.length() << "keyword(s)" << "|" << keywords;
        auto *metadataOperator = getMetadataOperator();
        int appendedCount = metadataOperator->appendKeywords(keywords);
        LOG_INFO << "Appended" << appendedCount << "keywords";

        if (appendedCount > 0) {
            signalKeywordsCountChanged();

            auto *basicModel = getBasicMetadataModel();
            m_CommandManager->submitItemForSpellCheck(basicModel, Common::SpellCheckFlags::Keywords);
        }

        return appendedCount;
    }

    bool ArtworkProxyBase::doClearKeywords() {
        LOG_DEBUG << "#";
        auto *metadataOperator = getMetadataOperator();
        bool result = metadataOperator->clearKeywords();
        if (result) {
            signalKeywordsCountChanged();
        }

        // to update fix spelling link
        spellCheckKeywords();

        return result;
    }

    QString ArtworkProxyBase::doGetKeywordsString() {
        auto *metadataOperator = getMetadataOperator();
        return metadataOperator->getKeywordsString();
    }

    void ArtworkProxyBase::doSuggestCorrections() {
        LOG_DEBUG << "#";
        auto *metadataOperator = getMetadataOperator();
        m_CommandManager->setupSpellCheckSuggestions(metadataOperator, -1, Common::SuggestionFlags::All);
    }

    void ArtworkProxyBase::doInitDescriptionHighlighting(QQuickTextDocument *document) {
        auto *keywordsModel = getBasicMetadataModel();
        SpellCheck::SpellCheckItemInfo *info = keywordsModel->getSpellCheckInfo();

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForDescription(document->textDocument(), colorsModel, keywordsModel);
        keywordsModel->notifyDescriptionSpellCheck();
    }

    void ArtworkProxyBase::doInitTitleHighlighting(QQuickTextDocument *document) {
        auto *keywordsModel = getBasicMetadataModel();
        SpellCheck::SpellCheckItemInfo *info = keywordsModel->getSpellCheckInfo();

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForTitle(document->textDocument(), colorsModel, keywordsModel);
        keywordsModel->notifyTitleSpellCheck();
    }

    void ArtworkProxyBase::doSpellCheckDescription() {
        LOG_DEBUG << "#";
        auto *keywordsModel = getBasicMetadataModel();
        if (!keywordsModel->getDescription().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Description);
        } else {
            keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Description);
        }
    }

    void ArtworkProxyBase::doSpellCheckTitle() {
        LOG_DEBUG << "#";
        auto *keywordsModel = getBasicMetadataModel();
        if (!keywordsModel->getTitle().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Title);
        } else {
            keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Title);
        }
    }

    void ArtworkProxyBase::doPlainTextEdit(const QString &rawKeywords) {
        LOG_DEBUG << "#";
        QStringList keywords = rawKeywords.trimmed().split(QChar(','), QString::SkipEmptyParts);

        auto *metadataOperator = getMetadataOperator();
        metadataOperator->setKeywords(keywords);

        spellCheckKeywords();

        signalKeywordsCountChanged();
    }

    bool ArtworkProxyBase::getHasTitleWordSpellError(const QString &word) {
        auto *keywordsModel = getBasicMetadataModel();
        return keywordsModel->hasTitleWordSpellError(word);
    }

    bool ArtworkProxyBase::getHasDescriptionWordSpellError(const QString &word) {
        auto *keywordsModel = getBasicMetadataModel();
        return keywordsModel->hasDescriptionWordSpellError(word);
    }

    void ArtworkProxyBase::spellCheckKeywords() {
        auto *basicModel = getBasicMetadataModel();
        m_CommandManager->submitItemForSpellCheck(basicModel, Common::SpellCheckFlags::Keywords);
    }
}
