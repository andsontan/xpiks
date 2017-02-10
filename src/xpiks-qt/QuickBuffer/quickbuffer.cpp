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
                         this, SLOT(spellCheckErrorsFixedHandler()));
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

    void QuickBuffer::userDictUpdateHandler(const QStringList &keywords) {
        doHandleUserDictChanged(keywords);
    }

    void QuickBuffer::userDictClearedHandler() {
        doHandleUserDictCleared();
    }

    void QuickBuffer::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
    }

    void QuickBuffer::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
    }

    void QuickBuffer::appendKeyword(const QString &keyword) {
        doAppendKeyword(keyword);
    }

    void QuickBuffer::pasteKeywords(const QStringList &keywords) {
        doAppendKeywords(keywords);
    }

    void QuickBuffer::clearKeywords() {
        doClearKeywords();
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

    void QuickBuffer::plainTextEdit(const QString &rawKeywords) {
        doPlainTextEdit(rawKeywords);
    }

    bool QuickBuffer::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool QuickBuffer::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void QuickBuffer::resetModel() {
        m_BasicModel.clearModel();
    }
}
