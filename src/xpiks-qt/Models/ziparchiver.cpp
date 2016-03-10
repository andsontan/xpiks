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

#include "ziparchiver.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/filenameshelpers.h"
#include "../Common/defines.h"

namespace Models {
    ZipArchiver::ZipArchiver() {
        m_ArchiveCreator = new QFutureWatcher<QStringList>(this);
        connect(m_ArchiveCreator, SIGNAL(resultReadyAt(int)), SLOT(archiveCreated(int)));
        connect(m_ArchiveCreator, SIGNAL(finished()), SLOT(allFinished()));
    }

    int ZipArchiver::getItemsCount() const {
        const QVector<Models::ArtworkMetadata *> items = getArtworkList();
        int size = items.size(), count = 0;
        for (int i = 0; i < size; ++i) {
            if (items[i]->hasVectorAttached()) {
                count++;
            }
        }

        return count;
    }

    void ZipArchiver::archiveCreated(int) {
        incProgress();
    }

    void ZipArchiver::allFinished() {
        LOG_INFO << "#";
        endProcessing();
    }

    void ZipArchiver::archiveArtworks() {
        LOG_DEBUG << "#";
        QHash<QString, QStringList> itemsWithSameName;
        fillFilenamesHash(itemsWithSameName);

        if (itemsWithSameName.empty()) {
            LOG_INFO << "No items to zip. Exiting...";
            return;
        }

        beginProcessing();
        restrictMaxThreads();

        QList<QStringList> items = itemsWithSameName.values();

        LOG_INFO << "Creating zip archives for" << items.length() << "item(s)";
        m_ArchiveCreator->setFuture(QtConcurrent::mapped(items, Helpers::zipFiles));
    }

    void ZipArchiver::fillFilenamesHash(QHash<QString, QStringList> &hash) {
        QVector<Models::ArtworkMetadata*> artworksList = getArtworkList();

        foreach (Models::ArtworkMetadata *metadata, artworksList) {
            const QString &filepath = metadata->getFilepath();

            QFileInfo fi(filepath);
            QString basename = fi.baseName();

            if (metadata->hasVectorAttached()) {
                if (!hash.contains(basename)) {
                    hash.insert(basename, QStringList());
                }

                hash[basename].append(filepath);
                hash[basename].append(metadata->getAttachedVectorPath());
            }
        }
    }

    void ZipArchiver::removeUnavailableItems(){
        int i =0;
        QVector<ArtworkMetadata*> m_ArtworksList_old=getArtworkList();
        for (auto it = m_ArtworksList_old.begin(); it!=m_ArtworksList_old.end(); it++, i++){
            if ((*it)->getIsRemoved()){
                m_ArtworksList_new.append(*it);
            }
        }
    }

    void ZipArchiver::UpdateMyself(){
        setArtworkList(m_ArtworksList_new);
        if (!m_ArtworksList_new.size())
            emit CloseWindow();
        m_ArtworksList_new.clear();

    }
}

