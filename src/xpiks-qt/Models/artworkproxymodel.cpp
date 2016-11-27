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

#include "artworkproxymodel.h"
#include "../Commands/commandmanager.h"

namespace Models {
    ArtworkProxyModel::ArtworkProxyModel(QObject *parent) :
        QObject(parent),
        m_ArtworkMetadata(nullptr)
    {
    }

    int ArtworkProxyModel::getKeywordsCount() const {
        Common::BasicKeywordsModel *keywordsModel = m_ArtworkMetadata->getBasicModel();
        return keywordsModel->getKeywordsCount();
    }

    void ArtworkProxyModel::setDescription(const QString &description)  {
        Q_ASSERT(m_ArtworkMetadata != nullptr);

        if (m_ArtworkMetadata->setDescription(description)) {
            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }

            emit descriptionChanged();
        }
    }

    void ArtworkProxyModel::setTitle(const QString &title) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);

        if (m_ArtworkMetadata->setTitle(title)) {
            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }

            emit titleChanged();
        }
    }

    void ArtworkProxyModel::spellCheckErrorsChangedHandler() {
        emit descriptionChanged();
        emit titleChanged();
    }

    void ArtworkProxyModel::editKeyword(int index, const QString &replacement) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_INFO << "index:" << index << "replacement:" << replacement;
        if (m_ArtworkMetadata->editKeyword(index, replacement)) {
            auto *keywordsModel = doGetBasicModel();
            m_CommandManager->submitKeywordForSpellCheck(keywordsModel, index);
        } else {
            LOG_INFO << "Failed to edit to" << replacement;
        }
    }

    void ArtworkProxyModel::removeKeywordAt(int keywordIndex) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_INFO << "keyword index:" << keywordIndex;
        if (m_ArtworkMetadata->removeKeywordAt(keywordIndex)) {
            emit keywordsCountChanged();
            LOG_INFO << "Keywords count:" << getKeywordsCount();
        }
    }

    void ArtworkProxyModel::removeLastKeyword() {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_DEBUG << "#";
        if (m_ArtworkMetadata->removeLastKeyword()) {
            emit keywordsCountChanged();
            LOG_INFO << "Keywords count:" << getKeywordsCount();
        }
    }

    void ArtworkProxyModel::appendKeyword(const QString &keyword) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_INFO << keyword;
        if (m_ArtworkMetadata->appendKeyword(keyword)) {
            emit keywordsCountChanged();

            auto *keywordsModel = doGetBasicModel();
            m_CommandManager->submitKeywordForSpellCheck(keywordsModel, keywordsModel->rowCount() - 1);
        } else {
            LOG_INFO << "Failed to append:" << keyword;
        }
    }

    void ArtworkProxyModel::pasteKeywords(const QStringList &keywords) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_INFO << keywords.length() << "keyword(s)" << "|" << keywords;
        int appendedCount = m_ArtworkMetadata->appendKeywords(keywords);
        LOG_INFO << "Appended" << appendedCount << "keywords";

        if (appendedCount > 0) {
            emit keywordsCountChanged();

            auto *keywordsModel = doGetBasicModel();
            m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Keywords);
        }
    }

    void ArtworkProxyModel::clearKeywords() {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_DEBUG << "#";
        if (m_ArtworkMetadata->clearKeywords()) {
            emit keywordsCountChanged();
        }
    }

    QString ArtworkProxyModel::getKeywordsString() {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        auto *keywordsModel = doGetBasicModel();
        return keywordsModel->getKeywordsString();
    }

    void ArtworkProxyModel::suggestCorrections() {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_DEBUG << "#";
        auto *keywordsModel = doGetBasicModel();
        m_CommandManager->setupSpellCheckSuggestions(keywordsModel, -1, Common::SuggestionFlags::All);
    }

    void ArtworkProxyModel::initDescriptionHighlighting(QQuickTextDocument *document) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        auto *keywordsModel = doGetBasicModel();
        SpellCheck::SpellCheckItemInfo *info = keywordsModel->getSpellCheckInfo();

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForDescription(document->textDocument(), colorsModel, keywordsModel);
        keywordsModel->notifyDescriptionSpellCheck();
    }

    void ArtworkProxyModel::initTitleHighlighting(QQuickTextDocument *document) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        auto *keywordsModel = doGetBasicModel();
        SpellCheck::SpellCheckItemInfo *info = keywordsModel->getSpellCheckInfo();

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForTitle(document->textDocument(), colorsModel, keywordsModel);
        keywordsModel->notifyTitleSpellCheck();
    }

    void ArtworkProxyModel::spellCheckDescription() {
        LOG_DEBUG << "#";
        auto *keywordsModel = doGetBasicModel();
        if (!keywordsModel->getDescription().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Description);
        } else {
            keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Description);
        }
    }

    void ArtworkProxyModel::spellCheckTitle() {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        LOG_DEBUG << "#";
        auto *keywordsModel = doGetBasicModel();
        if (!keywordsModel->getTitle().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Title);
        } else {
            keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Title);
        }
    }

    void ArtworkProxyModel::plainTextEdit(const QString &rawKeywords) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        QStringList keywords = rawKeywords.trimmed().split(QChar(','), QString::SkipEmptyParts);

        m_ArtworkMetadata->setKeywords(keywords);
        emit keywordsCountChanged();
    }

    bool ArtworkProxyModel::hasTitleWordSpellError(const QString &word) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        auto *keywordsModel = doGetBasicModel();
        return keywordsModel->hasTitleWordSpellError(word);
    }

    bool ArtworkProxyModel::hasDescriptionWordSpellError(const QString &word) {
        Q_ASSERT(m_ArtworkMetadata != nullptr);
        auto *keywordsModel = doGetBasicModel();
        return keywordsModel->hasDescriptionWordSpellError(word);
    }

    void ArtworkProxyModel::setSourceArtwork(QObject *artworkMetadata) {
        ArtworkMetadata *metadata = qobject_cast<ArtworkMetadata*>(artworkMetadata);
        Q_ASSERT(metadata != nullptr);

        if (m_ArtworkMetadata != nullptr) {
            auto *basicModel = m_ArtworkMetadata->getBasicModel();
            this->disconnect(basicModel);
        }

        m_ArtworkMetadata = metadata;

        auto *keywordsModel = metadata->getBasicModel();
        QObject::connect(keywordsModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));

        QObject::connect(keywordsModel, SIGNAL(completionsAvailable()),
                         this, SIGNAL(completionsAvailable()));

        emit descriptionChanged();
        emit titleChanged();
        emit keywordsCountChanged();
        emit imagePathChanged();
    }
}
