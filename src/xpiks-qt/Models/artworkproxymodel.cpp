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
        ArtworkProxyBase(),
        m_ArtworkMetadata(nullptr),
        m_ArtworkOriginalIndex(-1)
    {
    }

    ArtworkProxyModel::~ArtworkProxyModel() {
        doResetModel();
    }

    void ArtworkProxyModel::setDescription(const QString &description)  {
        if (doSetDescription(description)) {
            signalDescriptionChanged();

            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }
        }
    }

    void ArtworkProxyModel::setTitle(const QString &title) {
        if (doSetTitle(title)) {
            signalTitleChanged();

            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }
        }
    }

    void ArtworkProxyModel::spellCheckErrorsChangedHandler() {
        emit descriptionChanged();
        emit titleChanged();
    }

    void ArtworkProxyModel::afterSpellingErrorsFixedHandler() {
        // if squeezing took place after replace
        emit keywordsCountChanged();
    }

    void ArtworkProxyModel::editKeyword(int index, const QString &replacement) {
        doEditKeyword(index, replacement);
    }

    void ArtworkProxyModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
    }

    void ArtworkProxyModel::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
    }

    void ArtworkProxyModel::appendKeyword(const QString &keyword) {
        doAppendKeyword(keyword);
    }

    void ArtworkProxyModel::pasteKeywords(const QStringList &keywords) {
        doAppendKeywords(keywords);
    }

    void ArtworkProxyModel::clearKeywords() {
        doClearKeywords();
    }

    QString ArtworkProxyModel::getKeywordsString() {
        return doGetKeywordsString();
    }

    void ArtworkProxyModel::suggestCorrections() {
        doSuggestCorrections();
    }

    void ArtworkProxyModel::initDescriptionHighlighting(QQuickTextDocument *document) {
        doInitDescriptionHighlighting(document);
    }

    void ArtworkProxyModel::initTitleHighlighting(QQuickTextDocument *document) {
        doInitTitleHighlighting(document);
    }

    void ArtworkProxyModel::spellCheckDescription() {
        doSpellCheckDescription();
    }

    void ArtworkProxyModel::spellCheckTitle() {
        doSpellCheckTitle();
    }

    void ArtworkProxyModel::plainTextEdit(const QString &rawKeywords) {
        doPlainTextEdit(rawKeywords);
    }

    bool ArtworkProxyModel::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool ArtworkProxyModel::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void ArtworkProxyModel::setSourceArtwork(QObject *artworkMetadata, int originalIndex) {
        ArtworkMetadata *metadata = qobject_cast<ArtworkMetadata*>(artworkMetadata);
        Q_ASSERT(metadata != nullptr);

        updateCurrentArtwork();

        if (m_ArtworkMetadata != nullptr) {
            auto *basicModel = m_ArtworkMetadata->getBasicModel();
            this->disconnect(basicModel);
            m_ArtworkMetadata->release();
        }

        metadata->acquire();
        m_ArtworkMetadata = metadata;
        m_ArtworkOriginalIndex = originalIndex;

        auto *keywordsModel = metadata->getBasicModel();
        QObject::connect(keywordsModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));

        QObject::connect(keywordsModel, SIGNAL(completionsAvailable()),
                         this, SIGNAL(completionsAvailable()));

        QObject::connect(keywordsModel, SIGNAL(afterSpellingErrorsFixed()),
                         this, SLOT(afterSpellingErrorsFixedHandler()));

        emit descriptionChanged();
        emit titleChanged();
        emit keywordsCountChanged();
        emit imagePathChanged();
    }

    void ArtworkProxyModel::resetModel() {
        updateCurrentArtwork();
        doResetModel();
    }

    void ArtworkProxyModel::updateCurrentArtwork() {
        if (m_ArtworkOriginalIndex != -1) {
            m_CommandManager->updateArtworks(QVector<int>() << m_ArtworkOriginalIndex);
        }
    }

    void ArtworkProxyModel::doResetModel() {
        if (m_ArtworkMetadata != nullptr) {
            m_ArtworkMetadata->release();
        }

        m_ArtworkMetadata = nullptr;
        m_ArtworkOriginalIndex = -1;
    }
}
