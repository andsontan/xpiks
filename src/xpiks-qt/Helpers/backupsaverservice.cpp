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

#include "backupsaverservice.h"
#include <QThread>
#include "backupsaverworker.h"
#include "../Models/artworkmetadata.h"
#include "tempmetadatadb.h"

namespace Helpers {
    BackupSaverService::BackupSaverService():
        QObject()
    {
        m_BackupWorker = new BackupSaverWorker();
    }

    void BackupSaverService::startSaving() {
        Q_ASSERT(!m_BackupWorker->isRunning());

        QThread *thread = new QThread();
        m_BackupWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_BackupWorker, SLOT(process()));
        QObject::connect(m_BackupWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_BackupWorker, SIGNAL(stopped()), m_BackupWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelSaving()),
                         m_BackupWorker, SLOT(cancel()));

        thread->start();
    }

    void BackupSaverService::saveArtwork(Models::ArtworkMetadata *metadata) {
        TempMetadataCopy *copy = new TempMetadataCopy(metadata);
        m_BackupWorker->submitItem(copy);
    }
}

