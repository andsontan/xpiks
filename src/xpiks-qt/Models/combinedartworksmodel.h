#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

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

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QList>
#include "artiteminfo.h"
#include "abstractlistmodel.h"

namespace Models {
    class CombinedArtworksModel : public AbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(QStringList keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0) :
            AbstractListModel(parent)
        {}

        ~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    public:
        void initArtworks(const QList<ArtItemInfo*> &artworks);

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywords.clear(); std::copy(ek.begin(), ek.end(), std::back_inserter(m_CommonKeywords)); emit keywordsChanged(); }
        void initDescription(const QString &description) { setDescription(description); }
        void initTitle(const QString &title) { setTitle(title); }

    public:
        void recombineArtworks();

    public:
        const QStringList &getKeywords() const { return m_CommonKeywords; }
        void setKeywords(const QStringList& keywords) { m_CommonKeywords = keywords; emit keywordsChanged(); }
        const QString &getDescription() const { return m_ArtworkDescription; }
        void setDescription(const QString &description) { m_ArtworkDescription = description; emit descriptionChanged(); }
        const QString &getTitle() const { return m_ArtworkTitle; }
        void setTitle(const QString &title) { m_ArtworkTitle = title; emit titleChanged(); }
        int getKeywordsCount() const { return m_CommonKeywords.length(); }

    signals:
        void keywordsChanged();
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword() { removeKeywordAt(m_CommonKeywords.length() - 1); }
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void selectArtwork(int index);
        Q_INVOKABLE void deselectArtwork(int index);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE int getSelectedArtworksCount() const;
        Q_INVOKABLE int getArtworksCount() const { return m_ArtworksList.length(); }
        Q_INVOKABLE void saveSetKeywords();
        Q_INVOKABLE void saveAddKeywords();
        Q_INVOKABLE void resetModelData();
        Q_INVOKABLE QString getKeywordsString() { return m_CommonKeywords.join(','); }

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
        QStringList m_CommonKeywords;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
