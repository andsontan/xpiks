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

#ifndef ARTWORKSVIEWMODEL_H
#define ARTWORKSVIEWMODEL_H

#include <vector>
#include <functional>
#include "../Common/abstractlistmodel.h"
#include "metadataelement.h"
#include "../Common/baseentity.h"
#include "artworkmetadata.h"
#include "../Helpers/ifilenotavailablemodel.h"

namespace Models {
    class ArtworksViewModel:
            public Common::AbstractListModel,
            public virtual Common::BaseEntity,
            public Helpers::IFileNotAvailableModel
    {
        Q_OBJECT
        Q_PROPERTY(int artworksCount READ getArtworksCount NOTIFY artworksCountChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)

        enum ArtworksViewModel_Roles {
            FilepathRole = Qt::UserRole + 1,
            IsSelectedRole,
            HasVectorAttachedRole
        };

    public:
        ArtworksViewModel(QObject *parent=NULL);
        virtual ~ArtworksViewModel() {}

    public:
        virtual void setArtworks(std::vector<MetadataElement> &artworks);

    public:
        int getArtworksCount() const { return (int)m_ArtworksList.size(); }
        int getSelectedArtworksCount() const;

    public:
        Q_INVOKABLE void setArtworkSelected(int index, bool value);
        Q_INVOKABLE void removeSelectedArtworks() { doRemoveSelectedArtworks(); }
        Q_INVOKABLE void resetModel() { doResetModel(); }

    protected:
        bool isEmpty() const { return m_ArtworksList.empty(); }
        ArtworkMetadata *getArtworkMetadata(size_t i) const;
        virtual bool doRemoveSelectedArtworks();
        virtual void doResetModel();
        void processArtworks(std::function<bool (const MetadataElement &)> pred,
                             std::function<void (int, ArtworkMetadata *)> action) const;

#ifdef CORE_TESTS
    public:
#else
    protected:
#endif
        std::vector<MetadataElement> &getArtworksList() { return m_ArtworksList; }

    signals:
        void artworksCountChanged();
        void selectedArtworksCountChanged();
        void requestCloseWindow();

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

        // IFileNotAvailableModel interface
    public:
        virtual bool removeUnavailableItems();

        // AbstractListModel interface
    protected:
        virtual void removeInnerItem(int row);

    private:
        std::vector<MetadataElement> m_ArtworksList;
    };
}

#endif // ARTWORKSVIEWMODEL_H
