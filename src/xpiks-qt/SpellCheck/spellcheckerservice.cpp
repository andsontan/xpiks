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

#include "spellcheckerservice.h"
#include "../Models/artworkmetadata.h"
#include "spellcheckworker.h"
#include "spellcheckitem.h"
#include "../Common/defines.h"
#include "../Common/flags.h"

namespace SpellCheck {
    SpellCheckerService::SpellCheckerService():
        m_SpellCheckWorker(NULL),
        m_RestartRequired(false)
    {
    }

    SpellCheckerService::~SpellCheckerService() {
    }

    void SpellCheckerService::startService() {
        if (m_SpellCheckWorker != NULL) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        m_SpellCheckWorker = new SpellCheckWorker();

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

        QObject::connect(m_SpellCheckWorker, SIGNAL(destroyed(QObject*)),
                         this, SLOT(workerDestroyed(QObject*)));

        LOG_DEBUG << "starting thread...";
        thread->start();

        emit serviceAvailable(m_RestartRequired);
    }

    void SpellCheckerService::stopService() {
        LOG_DEBUG << "#";
        if (m_SpellCheckWorker != NULL) {
            m_SpellCheckWorker->stopWorking();
        } else {
            LOG_WARNING << "SpellCheckWorker is NULL";
        }
    }

    void SpellCheckerService::submitItem(Common::BasicKeywordsModel *itemToCheck) {
        this->submitItem(itemToCheck, Common::SpellCheckAll);
    }

    void SpellCheckerService::submitItem(Common::BasicKeywordsModel *itemToCheck, int flags) {
        if (m_SpellCheckWorker == NULL) { return; }

        itemToCheck->acquire();
        SpellCheckItem *item = new SpellCheckItem(itemToCheck, flags);
        itemToCheck->connectSignals(item);
        m_SpellCheckWorker->submitItem(item);
    }

    void SpellCheckerService::submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck) {
        if (m_SpellCheckWorker == NULL) { return; }

        QVector<SpellCheckItemBase *> items;
        int length = itemsToCheck.length();

        items.reserve(length);

        for (int i = 0; i < length; ++i) {
            Common::BasicKeywordsModel *itemToCheck = itemsToCheck.at(i);
            SpellCheckItem *item = new SpellCheckItem(itemToCheck, Common::SpellCheckAll);
            itemToCheck->connectSignals(item);
            items.append(item);
        }

        LOG_INFO << length << "item(s)";

        m_SpellCheckWorker->submitItems(items);
        m_SpellCheckWorker->submitItem(new SpellCheckSeparatorItem());
    }

    void SpellCheckerService::submitKeyword(Common::BasicKeywordsModel *itemToCheck, int keywordIndex) {
        if (m_SpellCheckWorker == NULL) { return; }

        itemToCheck->acquire();
        SpellCheckItem *item = new SpellCheckItem(itemToCheck, Common::SpellCheckKeywords, keywordIndex);
        itemToCheck->connectSignals(item);
        m_SpellCheckWorker->submitItem(item);
    }

    QStringList SpellCheckerService::suggestCorrections(const QString &word) const {
        if (m_SpellCheckWorker == NULL) { return QStringList(); }

        return m_SpellCheckWorker->retrieveCorrections(word);
    }

    void SpellCheckerService::restartWorker() {
        m_RestartRequired = true;
        stopService();
    }

    void SpellCheckerService::cancelCurrentBatch() {
        LOG_INFO << "#";

        if (m_SpellCheckWorker == NULL) { return; }

        m_SpellCheckWorker->cancelCurrentBatch();
    }

    bool SpellCheckerService::hasAnyPending() {
        bool hasPending = false;

        if (m_SpellCheckWorker != NULL) {
            hasPending = m_SpellCheckWorker->hasPendingJobs();
        }

        return hasPending;
    }

    void SpellCheckerService::workerFinished() {
        LOG_INFO << "#";
    }

    void SpellCheckerService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_SpellCheckWorker = NULL;

        if (m_RestartRequired) {
            LOG_INFO << "Restarting worker...";
            startService();
            m_RestartRequired = false;
        }
    }
}
