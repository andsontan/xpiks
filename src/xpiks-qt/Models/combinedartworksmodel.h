/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>
#include <QString>
#include <QList>
#include "artiteminfo.h"
#include "abstractlistmodel.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "../Commands/combinededitcommand.h"

namespace Models {

    class CombinedArtworksModel :
            public AbstractListModel,
            public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(QString author READ getAuthor WRITE setAuthor NOTIFY authorChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)
        Q_PROPERTY(int artworksCount READ getArtworksCount NOTIFY artworksCountChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0) :
            AbstractListModel(parent),
            m_CommonKeywordsModel(this)
        {}

        virtual ~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    public:
        void initArtworks(const QList<ArtItemInfo*> &artworks);

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywordsModel.reset(ek); m_CommonKeywordsSet.unite(QSet<QString>::fromList(m_CommonKeywordsModel.getKeywords())); }
        void initDescription(const QString &description) { setDescription(description); }
        void initTitle(const QString &title) { setTitle(title); }
        void initAuthor(const QString &author) { setAuthor(author); }

    public:
        void recombineArtworks();

    public:
        virtual void acceptSuggestedKeywords(const QStringList &keywords);

    public:
        void setKeywords(const QStringList& keywords) { m_CommonKeywordsModel.reset(keywords); }
        const QString &getDescription() const { return m_ArtworkDescription; }
        void setDescription(const QString &value) {
            if (m_ArtworkDescription != value) {
                m_ArtworkDescription = value;
                emit descriptionChanged();
            }
        }
        const QString &getTitle() const { return m_ArtworkTitle; }
        const QString &getAuthor() const { return m_ArtworkAuthor; }
        void setTitle(const QString &value) {
            if (m_ArtworkTitle != value) {
                m_ArtworkTitle = value;
                emit titleChanged();
            }
        }
        void setAuthor(const QString &value) {
            if (m_ArtworkAuthor != value) {
                m_ArtworkAuthor = value;
                emit authorChanged();
            }
        }
        int getKeywordsCount() const { return m_CommonKeywordsModel.rowCount(); }

    signals:
        void descriptionChanged();
        void titleChanged();
        void authorChanged();
        void keywordsCountChanged();
        void selectedArtworksCountChanged();
        void artworksCountChanged();

    public:
        int getSelectedArtworksCount() const;
        int getArtworksCount() const { return m_ArtworksList.length(); }

    public:
        Q_INVOKABLE QString removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void setArtworksSelected(int index, bool newState);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void saveSetKeywords() const;
        Q_INVOKABLE void saveAddKeywords() const;
        Q_INVOKABLE void resetModelData();
        Q_INVOKABLE QString getKeywordsString() { return m_CommonKeywordsModel.getKeywords().join(QChar(',')); }
        Q_INVOKABLE QObject *getKeywordsModel() {
            QObject *item = &m_CommonKeywordsModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

    private:
        void createCombinedEditCommand(Commands::CombinedEditType commandType) const;

    public:
        enum CombinedArtworksModelRoles {
            PathRole = Qt::UserRole + 1,
            IsSelectedRole
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int row);

    private:
        QList<ArtItemInfo*> m_ArtworksList;
        Common::BasicKeywordsModel m_CommonKeywordsModel;
        QSet<QString> m_CommonKeywordsSet;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QString m_ArtworkAuthor;
        bool m_IsModified;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
