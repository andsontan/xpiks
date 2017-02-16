/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include "quickbuffer.h"
#include "../Commands/commandmanager.h"
#include "icurrenteditable.h"
#include "../Models/uimanager.h"
#include "../QuickBuffer/currenteditableartwork.h"
#include "../Models/artitemsmodel.h"

namespace QuickBuffer {
    QuickBuffer::QuickBuffer(QObject *parent) :
        QObject(parent),
        ArtworkProxyBase(),
        m_BasicModel(m_HoldPlaceholder, this)
    {        
        m_BasicModel.setSpellCheckInfo(&m_SpellCheckInfo);

        QObject::connect(&m_BasicModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));

//        QObject::connect(&m_BasicModel, SIGNAL(completionsAvailable()),
//                         this, SIGNAL(completionsAvailable()));

        QObject::connect(&m_BasicModel, SIGNAL(afterSpellingErrorsFixed()),
                         this, SLOT(afterSpellingErrorsFixedHandler()));
    }

    QuickBuffer::~QuickBuffer() {
        // BUMP
    }

    void QuickBuffer::afterSpellingErrorsFixedHandler() {
        // if squeezing took place after replace
        signalKeywordsCountChanged();
    }

    void QuickBuffer::spellCheckErrorsChangedHandler() {
        emit descriptionChanged();
        emit titleChanged();
    }

    void QuickBuffer::userDictUpdateHandler(const QStringList &keywords, bool overwritten) {
        LOG_DEBUG << "#";
        doHandleUserDictChanged(keywords, overwritten);
    }

    void QuickBuffer::userDictClearedHandler() {
        LOG_DEBUG << "#";
        doHandleUserDictCleared();
    }

    void QuickBuffer::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
        emit isEmptyChanged();
    }

    void QuickBuffer::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
        emit isEmptyChanged();
    }

    void QuickBuffer::appendKeyword(const QString &keyword) {
        doAppendKeyword(keyword);
        emit isEmptyChanged();
    }

    void QuickBuffer::pasteKeywords(const QStringList &keywords) {
        doAppendKeywords(keywords);
        emit isEmptyChanged();
    }

    void QuickBuffer::clearKeywords() {
        doClearKeywords();
        emit isEmptyChanged();
    }

    QString QuickBuffer::getKeywordsString() {
        return doGetKeywordsString();
    }

    void QuickBuffer::initDescriptionHighlighting(QQuickTextDocument *document) {
        doInitDescriptionHighlighting(document);
    }

    void QuickBuffer::initTitleHighlighting(QQuickTextDocument *document) {
        doInitTitleHighlighting(document);
    }

    void QuickBuffer::spellCheckDescription() {
        doSpellCheckDescription();
    }

    void QuickBuffer::spellCheckTitle() {
        doSpellCheckTitle();
    }

    bool QuickBuffer::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool QuickBuffer::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void QuickBuffer::resetModel() {
        m_BasicModel.clearModel();
        emit isEmptyChanged();
    }

    bool QuickBuffer::copyToCurrentEditable() {
        LOG_DEBUG << "#";
        bool result = false;
        auto *uiManager = m_CommandManager->getUIManager();
        auto currentEditable = uiManager->getCurrentEditable();

        auto editableArtwork = std::dynamic_pointer_cast<CurrentEditableArtwork>(currentEditable);
        if (editableArtwork) {
            auto *artItemsModel = m_CommandManager->getArtItemsModel();
            artItemsModel->fillFromQuickBuffer(editableArtwork->getOriginalIndex());
            result = true;
        } else if (currentEditable) {
            currentEditable->setTitle(getTitle());
            currentEditable->setDescription(getDescription());
            currentEditable->setKeywords(getKeywords());

            currentEditable->spellCheck();
            currentEditable->update();

            result = true;
        } else {
            LOG_WARNING << "Nothing registered as current item";
        }

        return result;
    }

    bool QuickBuffer::getIsEmpty() {
        auto *model = getBasicMetadataModel();
        bool result = model->isTitleEmpty() || model->isDescriptionEmpty() || model->areKeywordsEmpty();
        return result;
    }

    void QuickBuffer::setFromCurrentEditable() {
        LOG_DEBUG << "#";
        auto *uiManager = m_CommandManager->getUIManager();
        auto currentEditable = uiManager->getCurrentEditable();

        if (currentEditable) {
            this->setTitle(currentEditable->getTitle());
            this->setDescription(currentEditable->getDescription());
            this->setKeywords(currentEditable->getKeywords());

            emit isEmptyChanged();
        } else {
            LOG_WARNING << "Nothing registered as current item";
        }
    }

    void QuickBuffer::setFromBasicModel(Common::BasicMetadataModel *model) {
        LOG_DEBUG << "#";
        Q_ASSERT(model != nullptr);

        this->setTitle(model->getTitle());
        this->setDescription(model->getDescription());
        this->setKeywords(model->getKeywords());

        emit isEmptyChanged();
    }
}
