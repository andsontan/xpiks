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

#include "metadataiocoordinator.h"
#include <QThread>
#include <QDebug>
#include "metadatareadingworker.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"

namespace MetadataIO {
    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity()
    {
    }

    void MetadataIOCoordinator::workerFinished(bool success) {
        qDebug() << "Metadata reading finished with status" << success;
        emit metadataReadingFinished();
    }

    void MetadataIOCoordinator::readMetadata(const QVector<Models::ArtworkMetadata *> &artworksToRead, bool ignoreBackups) {
        MetadataReadingWorker *worker = new MetadataReadingWorker(artworksToRead,
                                                                  m_CommandManager,
                                                                  ignoreBackups);
        QThread *thread = new QThread();
        worker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(worker, SIGNAL(finished(bool)), this, SLOT(workerFinished(bool)));

        thread->start();
    }
}

