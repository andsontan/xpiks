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

#include "ziparchiver.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/filenameshelpers.h"

namespace Models {
    ZipArchiver::ZipArchiver()
    {
        m_ArchiveCreator = new QFutureWatcher<QStringList>(this);
        connect(m_ArchiveCreator, SIGNAL(resultReadyAt(int)), SLOT(archiveCreated(int)));
        connect(m_ArchiveCreator, SIGNAL(finished()), SLOT(allFinished()));
    }

    void ZipArchiver::archiveCreated(int) {
        incProgress();
    }

    void ZipArchiver::allFinished() {
        endProcessing();
    }

    void ZipArchiver::archiveArtworks() {
        QHash<QString, QStringList> itemsWithSameName;
        fillFilenamesHash(itemsWithSameName);

        if (itemsWithSameName.empty()) {
            return;
        }

        beginProcessing();
        restrictMaxThreads();

        QList<QStringList> items = itemsWithSameName.values();
        m_ArchiveCreator->setFuture(QtConcurrent::mapped(items, Helpers::zipFiles));
    }

    void ZipArchiver::fillFilenamesHash(QHash<QString, QStringList> &hash) {
        QList<Models::ArtworkMetadata*> artworksList = getArtworkList();

        foreach (Models::ArtworkMetadata *metadata, artworksList) {
            QString filepath = metadata->getFilepath();
            QFileInfo fi(filepath);
            QString basename = fi.baseName();

            if (!hash.contains(basename)) {
                hash.insert(basename, QStringList());
            }

            hash[basename].append(filepath);

            QStringList vectors = Helpers::convertToVectorFilenames(QStringList() << filepath);

            foreach (const QString &item, vectors) {
                if (QFileInfo(item).exists()) {
                    hash[basename].append(item);
                    break;
                }
            }
        }
    }
}

