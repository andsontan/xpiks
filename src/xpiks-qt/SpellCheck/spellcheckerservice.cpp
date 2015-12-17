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

#include "spellcheckerservice.h"
#include <QThread>
#include <QDebug>

#include "../Models/artworkmetadata.h"
#include "spellcheckworker.h"
#include "spellcheckitem.h"
#include "../Common/defines.h"

namespace SpellCheck {
    SpellCheckerService::SpellCheckerService() {
        m_SpellCheckWorker = new SpellCheckWorker();
        m_WorkerIsAlive = false;
    }

    SpellCheckerService::~SpellCheckerService() {
        if (m_WorkerIsAlive && m_SpellCheckWorker && m_SpellCheckWorker->isRunning()) {
            m_SpellCheckWorker->cancelWork();
        }
    }

    void SpellCheckerService::startChecking() {
        Q_ASSERT(!m_SpellCheckWorker->isRunning());

        QThread *thread = new QThread();
        m_SpellCheckWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_SpellCheckWorker, SLOT(process()));
        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), m_SpellCheckWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelSpellChecking()),
                         m_SpellCheckWorker, SLOT(cancel()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(queueIsEmpty()),
                         this, SIGNAL(spellCheckQueueIsEmpty()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()),
                         this, SLOT(workerFinished()));

        thread->start();
        m_WorkerIsAlive = true;
    }

    void SpellCheckerService::submitItems(const QVector<ISpellCheckable *> &itemsToCheck) {
        if (!m_WorkerIsAlive) { return; }

        if (m_SpellCheckWorker != NULL && !m_SpellCheckWorker->isCancelled()) {
            QVector<SpellCheckItemBase *> items;
            int length = itemsToCheck.length();

            items.reserve(length);

            for (int i = 0; i < length; ++i) {
                SpellCheck::ISpellCheckable *itemToCheck = itemsToCheck.at(i);
                SpellCheckItem *item = new SpellCheckItem(itemToCheck);
                itemToCheck->connectSignals(item);
                items.append(item);
            }

            qInfo() << "SpellCheck service: about to submit" << length << "items";

            m_SpellCheckWorker->submitItems(items);
            m_SpellCheckWorker->submitItem(new SpellCheckSeparatorItem());
        }
    }

    void SpellCheckerService::submitKeyword(SpellCheck::ISpellCheckable *itemToCheck, int keywordIndex) {
        if (!m_WorkerIsAlive) { return; }

        Q_ASSERT(m_SpellCheckWorker != NULL);

        if (m_SpellCheckWorker != NULL && !m_SpellCheckWorker->isCancelled()) {
            SpellCheckItem *item = new SpellCheckItem(itemToCheck, keywordIndex);
            itemToCheck->connectSignals(item);
            m_SpellCheckWorker->submitItem(item);
        }
    }

    QStringList SpellCheckerService::suggestCorrections(const QString &word) const {
        if (!m_WorkerIsAlive) { return QStringList(); }

        if (m_SpellCheckWorker != NULL) {
            return m_SpellCheckWorker->retrieveCorrections(word);
        }

        return QStringList();
    }

    void SpellCheckerService::cancelCurrentBatch() {
        if (!m_WorkerIsAlive) { return; }

        if (m_SpellCheckWorker != NULL) {
            qInfo() << "SpellCheck service: cancelling current batch";
            m_SpellCheckWorker->cancelCurrentBatch();
        }
    }

    bool SpellCheckerService::hasAnyPending() {
        bool hasPending = false;

        if (m_WorkerIsAlive && (m_SpellCheckWorker != NULL)) {
            hasPending = m_SpellCheckWorker->hasPendingJobs();
        }

        return hasPending;
    }

    void SpellCheckerService::workerFinished() {
        qInfo() << "Spellcheck service went offline";
        m_WorkerIsAlive = false;
    }

    void SpellCheck::SpellCheckerService::stopChecking() {
        if (m_WorkerIsAlive) {
            qDebug() << "SpellCheck service: stopping checking...";
            m_SpellCheckWorker->cancelWork();
        }
    }

}
