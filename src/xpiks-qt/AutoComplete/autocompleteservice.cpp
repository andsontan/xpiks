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

#include "autocompleteservice.h"
#include <QThread>
#include "autocompleteworker.h"
#include "completionquery.h"
#include "../Common/flags.h"
#include "../Common/basickeywordsmodel.h"

namespace AutoComplete {
    AutoCompleteService::AutoCompleteService(AutoCompleteModel *autoCompleteModel, QObject *parent):
        QObject(parent),
        m_AutoCompleteWorker(NULL),
        m_AutoCompleteModel(autoCompleteModel),
        m_RestartRequired(false)
    {
    }

    AutoCompleteService::~AutoCompleteService() {
    }

    void AutoCompleteService::startService() {
        if (m_AutoCompleteWorker != NULL) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        m_AutoCompleteWorker = new AutoCompleteWorker();

        QThread *thread = new QThread();
        m_AutoCompleteWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_AutoCompleteWorker, SLOT(process()));
        QObject::connect(m_AutoCompleteWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_AutoCompleteWorker, SIGNAL(stopped()), m_AutoCompleteWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelAutoCompletion()),
                         m_AutoCompleteWorker, SLOT(cancel()));

        QObject::connect(m_AutoCompleteWorker, SIGNAL(stopped()),
                         this, SLOT(workerFinished()));

        QObject::connect(m_AutoCompleteWorker, SIGNAL(destroyed(QObject*)),
                         this, SLOT(workerDestroyed(QObject*)));

        LOG_DEBUG << "starting thread...";
        thread->start();

        emit serviceAvailable(m_RestartRequired);
    }

    void AutoCompleteService::stopService() {
        LOG_DEBUG << "#";
        if (m_AutoCompleteWorker != NULL) {
            m_AutoCompleteWorker->stopWorking();
        } else {
            LOG_WARNING << "AutoComplete Worker was NULL";
        }
    }

    bool AutoCompleteService::isBusy() const {
        bool isBusy = (m_AutoCompleteWorker != NULL) && m_AutoCompleteWorker->hasPendingJobs();
        return isBusy;
    }

    void AutoCompleteService::submitItem(QString *item) {
        Q_UNUSED(item);
    }

    void AutoCompleteService::submitItem(QString *item, int flags) {
        Q_UNUSED(flags);
        this->submitItem(item);
    }

    void AutoCompleteService::submitItems(const QVector<QString *> &items) {
        Q_UNUSED(items);
#if QT_NO_DEBUG
        LOG_WARNING << "Cannot use this API for autocomplete";
#else
        throw -1;
#endif
    }

    void AutoCompleteService::restartWorker() {
        m_RestartRequired = true;
        stopService();
    }

    void AutoCompleteService::findKeywordCompletions(const QString &prefix, QObject *notifyObject) {
        if (m_AutoCompleteWorker == NULL) {
            LOG_WARNING << "Worker is NULL";
            return;
        }

        LOG_INFO << "Received:" << prefix;
        QString requestPrefix = prefix.toLower();
        LOG_INFO << "Requesting for" << requestPrefix;
        std::shared_ptr<CompletionQuery> query(new CompletionQuery(requestPrefix, m_AutoCompleteModel),
                                               [](CompletionQuery *cq) { cq->deleteLater(); });

        Common::BasicKeywordsModel *basicKeywordsModel = qobject_cast<Common::BasicKeywordsModel*>(notifyObject);
        Q_ASSERT(basicKeywordsModel != NULL);

        QObject::connect(query.get(), SIGNAL(completionsAvailable()), basicKeywordsModel, SIGNAL(completionsAvailable()));
        QObject::connect(query.get(), SIGNAL(completionsAvailable()), m_AutoCompleteModel, SLOT(completionsArrived()));

        m_AutoCompleteWorker->submitItem(query);
    }

    void AutoCompleteService::workerFinished() {
        LOG_INFO << "#";
    }

    void AutoCompleteService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_AutoCompleteWorker = NULL;

        if (m_RestartRequired) {
            LOG_INFO << "Restarting worker...";
            startService();
            m_RestartRequired = false;
        }
    }
}
