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
#include <QFutureWatcher>
#include "../Common/baseentity.h"

namespace Models {
    class WarningsInfo;
    class ArtworkMetadata;
    class ArtItemInfo;

    class WarningsManager : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int warningsCount READ getWarningsCount NOTIFY warningsCountChanged)
    public:
        WarningsManager(QObject *parent = 0);

        virtual ~WarningsManager();

    public:
        enum WarningManagerRoles {
            ImagePathRole = Qt::UserRole + 1,
            WarningsListRole,
            ItemIndexRole
        };

    signals:
        void warningsCountChanged();

    public slots:
        void onCheckWarnings(const QVector<ArtItemInfo*> &artworks);

    private slots:
        void warningsCheckFinished();

    public:
        int getWarningsCount() const { return m_WarningsList.length(); }
        void checkForWarnings(const QVector<ArtItemInfo *> &artworks);
        Q_INVOKABLE void recheckItem(int itemIndex);

        bool checkItem(WarningsInfo *metadata) const;

    private:
        void clearModel();
        QVector<WarningsInfo*> doCheckItems(const QVector<WarningsInfo*> &items) const;
        bool checkDimensions(WarningsInfo *wi) const;
        bool checkKeywordsCount(WarningsInfo *wi) const;
        bool checkDescriptionLength(WarningsInfo *wi) const;
        bool checkTitleWordsCount(WarningsInfo *wi) const;
        bool checkSpellCheckErrors(WarningsInfo *wi) const;
        void initConstraintsFromSettings();

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QVector<WarningsInfo*> m_WarningsList;
        QFutureWatcher<QVector<WarningsInfo*> > *m_CheckingWatcher;
        double m_MinimumMegapixels;
        int m_MaximumKeywordsCount;
        int m_MaximumDescriptionLength;
    };
}

#endif // WARNINGSMANAGER_H
