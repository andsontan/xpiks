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

#ifndef WARNINGSMANAGER_H
#define WARNINGSMANAGER_H

#include <QAbstractListModel>
#include "warningsinfo.h"
#include "artworkmetadata.h"
#include "../Helpers/globalimageprovider.h"
#include "../Common/baseentity.h"

namespace Models {
    class WarningsManager : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int warningsCount READ getWarningsCount NOTIFY warningsCountChanged)
    public:
        WarningsManager(QObject *parent = 0) :
            QAbstractListModel(parent)
        { }

        virtual ~WarningsManager() {}

    public:
        enum WarningManagerRoles {
            ImagePathRole = Qt::UserRole + 1,
            WarningsListRole
        };

    signals:
        void warningsCountChanged();

    public slots:
        void onCheckWarnings(const QList<ArtworkMetadata*> &artworks) { checkForWarnings(artworks); }

    public:
        int getWarningsCount();
        void checkForWarnings(const QList<ArtworkMetadata*> &artworks);
        void recheckItems();
        void setImageProvider(Helpers::GlobalImageProvider *imageProvider) { Q_ASSERT(imageProvider != NULL); m_ImageProvider = imageProvider; }

    private:
        void checkItem(WarningsInfo *metadata);
        bool checkDimensions(WarningsInfo *wi, ArtworkMetadata *am) const;
        bool checkKeywordsCount(WarningsInfo *wi, ArtworkMetadata *am) const;
        bool checkDescriptionLength(WarningsInfo *wi, ArtworkMetadata *am) const;
        void initConstraintsFromSettings();

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QList<WarningsInfo*> m_WarningsList;
        QList<WarningsInfo*> m_WarningsBufferList;
        Helpers::GlobalImageProvider *m_ImageProvider;
        double m_MinimumMegapixels;
        int m_MaximumKeywordsCount;
        int m_MaximumDescriptionLength;
    };
}

#endif // WARNINGSMANAGER_H
