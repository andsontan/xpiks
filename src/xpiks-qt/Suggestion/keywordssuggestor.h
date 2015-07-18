/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef KEYWORDSSUGGESTOR_H
#define KEYWORDSSUGGESTOR_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include "suggestionartwork.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "ikeywordssuggesteable.h"

namespace Suggestion {
    class KeywordsSuggestor : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        KeywordsSuggestor(QObject *parent=NULL) :
            QObject(parent),
            m_SelectedArtworksCount(0),
            m_Suggesteable(NULL)
        {}

        ~KeywordsSuggestor() { qDeleteAll(m_Suggestions); }

    public:
        void setSuggestedArtworks(const QList<SuggestionArtwork*> &suggestedArtworks);
        void setSuggesteable(IKeywordsSuggesteable *suggesteable) { m_Suggesteable = suggesteable; }
        void clear();

    public:
        Q_INVOKABLE void removeSuggestedKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeSuggestedLastKeyword();
        Q_INVOKABLE void setArtworkSelected(int index, bool newState);
        Q_INVOKABLE void suggestKeywords();
        Q_INVOKABLE void close() { clear(); }

        Q_INVOKABLE QObject *getSuggestedKeywords() {
            QObject *item = &m_SuggestedKeywords;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE QObject *getAllOtherKeywords() {
            QObject *item = &m_AllOtherKeywords;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

    public:
        enum KeywordsSuggestorRoles {
            UrlRole = Qt::UserRole + 1,
            IsSelectedRole
        };

        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        void accountKeywords(const QStringList &keywords, int sign);
        QSet<QString> getSelectedArtworksKeywords() const;
        void updateSuggestedKeywords();

    private:
        QHash<QString, int> m_KeywordsHash;
        QList<SuggestionArtwork *> m_Suggestions;
        Common::BasicKeywordsModel m_SuggestedKeywords;
        Common::BasicKeywordsModel m_AllOtherKeywords;
        IKeywordsSuggesteable *m_Suggesteable;
        int m_SelectedArtworksCount;
    };
}

#endif // KEYWORDSSUGGESTOR_H
