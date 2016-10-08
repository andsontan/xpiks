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

#ifndef WARNINGSSERVICE_H
#define WARNINGSSERVICE_H

#include <QObject>
#include "../Common/baseentity.h"
#include "../Common/iservicebase.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"
#include "warningssettingsmodel.h"

namespace Warnings {
    class WarningsCheckingWorker;

    class WarningsService:
        public QObject,
        public Common::BaseEntity,
        public Common::IServiceBase<Models::ArtworkMetadata, Common::WarningsCheckFlags>
    {
    Q_OBJECT

    public:
        explicit WarningsService(QObject *parent=0);
        virtual ~WarningsService() {}

    public:
        void initWarningsSettings();
        const WarningsSettingsModel *getWarningsSettingsModel() const { return &m_WarningsSettingsModel; }

    public:
        virtual void startService();
        virtual void stopService();

        virtual bool isAvailable() const { return true; }
        virtual bool isBusy() const;

        virtual void submitItem(Models::ArtworkMetadata *item);
        virtual void submitItem(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags);
        virtual void submitItems(const QVector<Models::ArtworkMetadata *> &items);
        virtual void setCommandManager(Commands::CommandManager *commandManager);

    private slots:
        void workerDestoyed(QObject *object);
        void workerStopped();
        void updateWarningsSettings();

#ifdef INTEGRATION_TESTS

    signals:
        void queueIsEmpty();

#endif

    private:
        WarningsCheckingWorker *m_WarningsWorker;
        WarningsSettingsModel m_WarningsSettingsModel;
    };
}

#endif // WARNINGSSERVICE_H
