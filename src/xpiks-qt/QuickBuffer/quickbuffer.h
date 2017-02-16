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

#ifndef QUICKBUFFER_H
#define QUICKBUFFER_H

#include <QObject>
#include <QQmlEngine>
#include "../Models/artworkproxybase.h"
#include "../Common/basicmetadatamodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"

namespace QuickBuffer {
    class QuickBuffer : public QObject, public Models::ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(bool isEmpty READ getIsEmpty NOTIFY isEmptyChanged)

    public:
        explicit QuickBuffer(QObject *parent = 0);
        virtual ~QuickBuffer();

    signals:
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();
        void isEmptyChanged();

    protected:
        virtual void signalDescriptionChanged() override { emit descriptionChanged(); }
        virtual void signalTitleChanged() override { emit titleChanged(); }
        virtual void signalKeywordsCountChanged() override { emit keywordsCountChanged(); }

    public slots:
        void afterSpellingErrorsFixedHandler();
        void spellCheckErrorsChangedHandler();
        void userDictUpdateHandler(const QStringList &keywords, bool overwritten);
        void userDictClearedHandler();

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE QString getKeywordsString();
        Q_INVOKABLE void initDescriptionHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void spellCheckDescription();
        Q_INVOKABLE void spellCheckTitle();
        Q_INVOKABLE bool hasTitleWordSpellError(const QString &word);
        Q_INVOKABLE bool hasDescriptionWordSpellError(const QString &word);
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE bool copyToCurrentEditable();

    public:
        bool getIsEmpty();
        void setFromCurrentEditable();
        void setFromBasicModel(Common::BasicMetadataModel *model);

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() override { return &m_BasicModel; }
        virtual Common::IMetadataOperator *getMetadataOperator() override { return &m_BasicModel; }

    private:
        Common::Hold m_HoldPlaceholder;
        Common::BasicMetadataModel m_BasicModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
    };
}

#endif // QUICKBUFFER_H
