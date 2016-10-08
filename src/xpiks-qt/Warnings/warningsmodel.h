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

#ifndef WARNINGSMODEL_H
#define WARNINGSMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include "../Common/baseentity.h"

namespace Warnings {
    class WarningsSettingsModel;

    class WarningsModel: public QSortFilterProxyModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int warningsCount READ getWarningsCount NOTIFY warningsCountChanged)
    public:
        WarningsModel(QObject *parent=0);

    public:
        void setWarningsSettingsModel(const WarningsSettingsModel *warningsSettingsModel) {
            m_WarningsSettingsModel = warningsSettingsModel;
            Q_ASSERT(warningsSettingsModel != nullptr);
        }

    public:
        int getWarningsCount() const { return rowCount(); }

    public:
        Q_INVOKABLE void setShowSelected() { m_ShowOnlySelected = true; update(); }
        Q_INVOKABLE void resetShowSelected() { m_ShowOnlySelected = false; update(); }
        Q_INVOKABLE QStringList describeWarnings(int index) const;
        Q_INVOKABLE void update();
        Q_INVOKABLE int getOriginalIndex(int index) const;

    signals:
        void warningsCountChanged();

    private slots:
        void sourceRowsRemoved(QModelIndex,int,int);
        void sourceRowsInserted(QModelIndex,int,int);
        void sourceModelReset();

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    public:
        virtual void setSourceModel(QAbstractItemModel *sourceModel);

    private:
        bool m_ShowOnlySelected;
        const WarningsSettingsModel *m_WarningsSettingsModel;
    };
}

#endif // WARNINGSMODEL_H
