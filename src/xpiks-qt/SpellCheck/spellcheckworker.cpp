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

#include "spellcheckworker.h"

#include <QDebug>
#include "spellcheckitem.h"

namespace SpellCheck {
    SpellCheckWorker::SpellCheckWorker(QObject *parent) : QObject(parent) {
    }

    void SpellCheckWorker::submitItemToCheck(SpellCheckItem *item) {
        m_Mutex.lock();
        {
            m_Queue.append(item);
        }
        m_Mutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::submitItemsToCheck(const QList<SpellCheckItem *> &items) {
        m_Mutex.lock();
        {
            m_Queue.append(items);
        }
        m_Mutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::cancelCurrentRequests() {
        m_Mutex.lock();
        {
            qDeleteAll(m_Queue);
        }
        m_Queue.clear();
        m_Mutex.unlock();
    }

    void SpellCheckWorker::initHunspell() {
        // TODO: implement this
    }

    QStringList SpellCheckWorker::suggestCorrections(const QString &word) {
        // TODO: implement this
    }

    void SpellCheckWorker::spellcheckLoop() {
        for (;;) {
            if (m_Cancel) {
                break;
            }

            m_Mutex.lock();

            if (m_Queue.isEmpty()) {
                m_WaitAnyItem.wait(&m_Mutex);

                // can be cleared by clearCurrectRequests()
                if (m_Queue.isEmpty()) {
                    m_Mutex.unlock();
                    continue;
                }
            }

            SpellCheckItem *item = m_Queue.first();
            m_Queue.removeFirst();

            m_Mutex.unlock();

            if (item == NULL) { break; }

            try {
                processOneRequest(item);
            }
            catch (...) {
                qDebug() << "Error while processing spellcheck";
            }

            delete item;
        }
    }

    void SpellCheckWorker::processOneRequest(const SpellCheckItem *item) {
        // TODO: implement this
    }

    void SpellCheckWorker::process() {
        initHunspell();
        spellcheckLoop();
    }

    void SpellCheckWorker::cancel() {
        m_Cancel = true;
        submitItemToCheck(NULL);
    }
}
