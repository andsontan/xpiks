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

#include "locallibraryqueryengine.h"

#include <QThread>
#include "libraryqueryworker.h"
#include "../Common/defines.h"
#define MAX_LOCAL_RESULTS 100

namespace Suggestion {
    LocalLibraryQueryEngine::LocalLibraryQueryEngine(int engineID, LocalLibrary *localLibrary):
        SuggestionQueryEngineBase(engineID),
        m_LocalLibrary(localLibrary)
    {
    }

    void LocalLibraryQueryEngine::submitQuery(const QStringList &queryKeywords, QueryResultsType resultsType) {
        LOG_DEBUG << queryKeywords;
        Q_UNUSED(resultsType);
        LibraryQueryWorker *worker = new LibraryQueryWorker(m_LocalLibrary, queryKeywords, MAX_LOCAL_RESULTS);
        QThread *thread = new QThread();
        worker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelAllQueries()),
                         worker, SLOT(cancel()));

        QObject::connect(worker, SIGNAL(resultsFound()),
                         this, SLOT(resultsFoundHandler()));

        thread->start();
    }

    void LocalLibraryQueryEngine::resultsFoundHandler() {
        LibraryQueryWorker *worker = qobject_cast<LibraryQueryWorker*>(sender());
        setResults(worker->getResults());
        worker->doShutdown();
        emit resultsAvailable();
    }
}
