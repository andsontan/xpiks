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

namespace Models {
    class MiniKeywordsModel : public QAbstractListModel, public Common::BaseEntity {
        Q_OBJECT
    public:
        MiniKeywordsModel(QObject *parent) :
            QAbstractListModel(parent)
        { }

    public:
        enum CombinedArtworksModelRoles {
            KeywordRole = Qt::UserRole + 1
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_KeywordsList.length(); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const {
            if (index.row() < 0 || index.row() >= m_KeywordsList.count()) return QVariant();
            if (role == KeywordRole) { return m_KeywordsList[index.row()]; }
            return QVariant();
        }
        const QStringList &getKeywords() const { return m_KeywordsList; }
        void reset(const QStringList &items) { beginResetModel(); m_KeywordsList.clear(); m_KeywordsList.append(items); endResetModel(); }
        void appendKeyword(const QString &keyword) { beginInsertRows(QModelIndex(), rowCount(), rowCount());
                                                    m_KeywordsList.append(keyword); endInsertRows(); }
        bool removeKeyword(int index, QString &keyword) { bool indexValid = index >= 0 && index < m_KeywordsList.length(); if (indexValid) {
                beginRemoveRows(QModelIndex(), index, index); keyword = m_KeywordsList.takeAt(index); endRemoveRows(); }
                return indexValid;
            }
        bool removeLastKeyword(QString &keyword) { return removeKeyword(m_KeywordsList.length() - 1, keyword); }

    protected:
        QHash<int, QByteArray> roleNames() const {
            QHash<int, QByteArray> roles; roles[KeywordRole] = "keyword"; return roles;
        }

    private:
        QStringList m_KeywordsList;
    };

    class CombinedArtworksModel : public AbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(QString author READ getAuthor WRITE setAuthor NOTIFY authorChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0) :
            AbstractListModel(parent),
            m_CommonKeywordsModel(parent)
        {}

        ~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

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

    public:
        int getSelectedArtworksCount() const;

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void selectArtwork(int index);
        Q_INVOKABLE void deselectArtwork(int index);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE int getArtworksCount() const { return m_ArtworksList.length(); }
        Q_INVOKABLE void saveSetKeywords();
        Q_INVOKABLE void saveAddKeywords();
        Q_INVOKABLE void resetModelData();
        Q_INVOKABLE QString getKeywordsString() { return m_CommonKeywordsModel.getKeywords().join(QChar(',')); }
        Q_INVOKABLE QObject *getKeywordsModel() {
            QObject *item = &m_CommonKeywordsModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

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
        MiniKeywordsModel m_CommonKeywordsModel;
        QSet<QString> m_CommonKeywordsSet;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QString m_ArtworkAuthor;
        bool m_IsModified;
    };
}


Q_DECLARE_METATYPE(Models::MiniKeywordsModel*)

#endif // COMBINEDARTWORKSMODEL_H
