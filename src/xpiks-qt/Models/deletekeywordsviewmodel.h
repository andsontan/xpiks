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

#ifndef DELETEKEYWORDSVIEWMODEL_H
#define DELETEKEYWORDSVIEWMODEL_H

#include <QQmlEngine>
#include <QHash>
#include "../Common/hold.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "../Models/artworksviewmodel.h"

namespace Models {
    class DeleteKeywordsViewModel: public Models::ArtworksViewModel
    {
        Q_OBJECT
        Q_PROPERTY(int commonKeywordsCount READ getCommonKeywordsCount NOTIFY commonKeywordsCountChanged)
        Q_PROPERTY(int keywordsToDeleteCount READ getKeywordsToDeleteCount NOTIFY keywordsToDeleteCountChanged)
        Q_PROPERTY(bool caseSensitive READ getCaseSensitive WRITE setCaseSensitive NOTIFY caseSensitiveChanged)

    public:
        DeleteKeywordsViewModel(QObject *parent=nullptr);
        virtual ~DeleteKeywordsViewModel() {}

    public:
        int getCommonKeywordsCount() { return m_CommonKeywordsModel.getKeywordsCount(); }
        int getKeywordsToDeleteCount() { return m_KeywordsToDeleteModel.getKeywordsCount(); }
        bool getCaseSensitive() const { return m_CaseSensitive; }
        void setCaseSensitive(bool value) {
            if (m_CaseSensitive != value) {
                m_CaseSensitive = value;
                emit caseSensitiveChanged();
            }
        }

    signals:
        void commonKeywordsCountChanged();
        void keywordsToDeleteCountChanged();
        void caseSensitiveChanged();

    public:
        virtual void setArtworks(std::vector<MetadataElement> &artworks);
        virtual bool removeUnavailableItems();

#ifdef CORE_TESTS
    public:
        bool containsCommonKeyword(const QString &keyword) { return m_CommonKeywordsModel.containsKeyword(keyword); }
#endif

    protected:
        virtual bool doRemoveSelectedArtworks();
        virtual void doResetModel();

    public:
        Q_INVOKABLE QObject *getCommonKeywordsModel() {
            QObject *item = &m_CommonKeywordsModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE QObject *getKeywordsToDeleteModel() {
            QObject *item = &m_KeywordsToDeleteModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE void removeKeywordToDeleteAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeywordToDelete();
        Q_INVOKABLE void clearKeywordsToDelete();
        Q_INVOKABLE QString removeCommonKeywordAt(int keywordIndex);
        Q_INVOKABLE void appendKeywordToDelete(const QString &keyword);
        Q_INVOKABLE void pasteKeywordsToDelete(const QStringList &keywords);
        Q_INVOKABLE void deleteKeywords();

    private:
        void recombineKeywords();
        void fillKeywordsHash(QHash<QString, int> &keywordsHash);

    private:
        Common::Hold m_HoldForDeleters;
        Common::Hold m_HoldForCommon;
        Common::BasicKeywordsModel m_KeywordsToDeleteModel;
        Common::BasicKeywordsModel m_CommonKeywordsModel;
        bool m_CaseSensitive;
    };
}

#endif // DELETEKEYWORDSVIEWMODEL_H
