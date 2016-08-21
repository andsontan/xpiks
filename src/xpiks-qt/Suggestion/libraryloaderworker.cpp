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

#include <QFile>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QDataStream>
#include "libraryloaderworker.h"
#include "locallibrary.h"
#include "../Common/defines.h"

namespace Suggestion {
    LibraryLoaderWorker::LibraryLoaderWorker(Suggestion::LocalLibrary *localLibrary, const QString &filepath, LoadOption option):
        m_LocalLibrary(localLibrary),
        m_Filepath(filepath),
        m_Option(option)
    {
    }

    void LibraryLoaderWorker::process() {
        if (m_Option == Save) {
            write();
        } else if (m_Option == Load) {
            read();
        } else if (m_Option == Clean) {
            cleanup();
        }

        emit stopped();
    }

    void LibraryLoaderWorker::read() {
        LOG_DEBUG << "Loading library...";

        QFile file(m_Filepath);
        if (file.open(QIODevice::ReadOnly)) {
            QHash<QString, QStringList> dict;

            QDataStream in(&file);   // read the data
            in >> dict;
            file.close();

            m_LocalLibrary->swap(dict);
        }
    }

    void LibraryLoaderWorker::write() {
        LOG_DEBUG << "Saving library...";

        m_LocalLibrary->saveToFile();
    }

    void LibraryLoaderWorker::cleanup() {
        LOG_DEBUG << "#";
        m_LocalLibrary->cleanupTrash();
    }
}

