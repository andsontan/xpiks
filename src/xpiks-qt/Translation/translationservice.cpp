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

#include "translationservice.h"
#include <QThread>
#include "translationworker.h"
#include "translationquery.h"

namespace Translation {
    TranslationService::TranslationService(TranslationManager &manager, QObject *parent) :
        QObject(parent),
        m_TranslationManager(manager),
        m_TranslationWorker(nullptr),
        m_RestartRequired(false)
    {
    }

    void TranslationService::startService() {
        if (m_TranslationWorker != nullptr) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        m_TranslationWorker = new TranslationWorker();

        QThread *thread = new QThread();
        m_TranslationWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_TranslationWorker, SLOT(process()));
        QObject::connect(m_TranslationWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_TranslationWorker, SIGNAL(stopped()), m_TranslationWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(m_TranslationWorker, SIGNAL(stopped()),
                         this, SLOT(workerFinished()));

        QObject::connect(m_TranslationWorker, SIGNAL(destroyed(QObject*)),
                         this, SLOT(workerDestroyed(QObject*)));

        LOG_DEBUG << "starting thread...";
        thread->start();
    }

    void TranslationService::stopService() {
        LOG_DEBUG << "#";
        if (m_TranslationWorker != nullptr) {
            m_TranslationWorker->stopWorking();
        } else {
            LOG_WARNING << "TranslationWorker was NULL";
        }
    }

    bool TranslationService::isBusy() const {
        bool isBusy = (m_TranslationWorker != NULL) && m_TranslationWorker->hasPendingJobs();
        return isBusy;
    }

    void TranslationService::submitItem(QString *item) {
        Q_UNUSED(item);
    }

    void TranslationService::submitItem(QString *item, int flags) {
        Q_UNUSED(flags);
        this->submitItem(item);
    }

    void TranslationService::submitItems(const QVector<QString *> &items) {
        Q_UNUSED(items);
#if QT_NO_DEBUG
        LOG_WARNING << "Cannot use this API for translation";
#else
        Q_ASSERT(false);
#endif
    }

    void TranslationService::selectDictionary(const QString &dictionaryPath) {
        Q_ASSERT(m_TranslationWorker != nullptr);
        m_TranslationWorker->selectDictionary(dictionaryPath);
    }

    void TranslationService::translate(const QString &what) {
        LOG_INFO << what;

        QStringList words = what.split(QChar(' '), QString::SkipEmptyParts);
        QString wordToTranslate = words.isEmpty() ? "" : words.last();

        std::shared_ptr<TranslationQuery> query(new TranslationQuery(wordToTranslate),
                                                [](TranslationQuery *tq) { tq->deleteLater(); });
        QObject::connect(query.get(), SIGNAL(translationAvailable()), &m_TranslationManager, SLOT(translationArrived()));

        m_TranslationWorker->submitItem(query);
    }

    void TranslationService::workerFinished() {
        LOG_INFO << "#";
    }

    void TranslationService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_TranslationWorker = nullptr;

        if (m_RestartRequired) {
            LOG_INFO << "Restarting worker...";
            startService();
            m_RestartRequired = false;
        }
    }
}
