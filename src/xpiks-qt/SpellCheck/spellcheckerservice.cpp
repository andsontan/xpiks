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

#include "../Models/artworkmetadata.h"
#include "spellcheckworker.h"
#include "spellcheckitem.h"

namespace SpellCheck {
    SpellCheckerService::SpellCheckerService() {
        m_SpellCheckWorker = new SpellCheckWorker();
    }

    void SpellCheckerService::startChecking() {
        QThread *thread = new QThread();
        m_SpellCheckWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_SpellCheckWorker, SLOT(process()));
        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), m_SpellCheckWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelSpellChecking()),
                         m_SpellCheckWorker, SLOT(cancel()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(queueIsEmpty()),
                         this, SLOT(workerQueueIsEmpty()));

        thread->start();
    }

    void SpellCheckerService::submitItems(const QList<ISpellCheckable *> &itemsToCheck) {
        Q_ASSERT(m_SpellCheckWorker != NULL);

        QList<SpellCheckItemBase *> items;

        foreach (SpellCheck::ISpellCheckable *itemToCheck, itemsToCheck) {
            SpellCheckItem *item = new SpellCheckItem(itemToCheck);
            itemToCheck->connectSignals(item);
            items.append(item);
        }

        m_SpellCheckWorker->submitItemsToCheck(items);
        m_SpellCheckWorker->submitItemToCheck(new SpellCheckSeparatorItem());
    }

    void SpellCheckerService::submitKeyword(SpellCheck::ISpellCheckable *itemToCheck, int keywordIndex) {
        Q_ASSERT(m_SpellCheckWorker != NULL);

        SpellCheckItem *item = new SpellCheckItem(itemToCheck, keywordIndex);
        itemToCheck->connectSignals(item);
        m_SpellCheckWorker->submitItemToCheck(item);
    }

    QStringList SpellCheckerService::suggestCorrections(const QString &word) const {
        return m_SpellCheckWorker->retrieveCorrections(word);
    }

    void SpellCheckerService::cancelCurrentBatch() {
        m_SpellCheckWorker->cancelCurrentBatch();
    }

    bool SpellCheckerService::hasAnyPending() {
        bool hasPending = m_SpellCheckWorker->hasPendingJobs();
        return hasPending;
    }

    void SpellCheckerService::workerQueueIsEmpty() {
        emit spellCheckQueueIsEmpty();
    }
}
