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

#ifndef ITEMPROCESSINGWORKER_H
#define ITEMPROCESSINGWORKER_H

#include <QWaitCondition>
#include <QMutex>
#include <QVector>
#include "../Common/defines.h"

namespace Common {
    template<typename T>
    class ItemProcessingWorker
    {
    public:
        ItemProcessingWorker():
            m_Cancel(false),
            m_IsRunning(false)
        { }
        virtual ~ItemProcessingWorker() { qDeleteAll(m_Queue); }

    public:
        void submitItem(T *item) {
            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.isEmpty();
                m_Queue.append(item);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void submitItems(const QVector<T*> &items) {
            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.isEmpty();
                m_Queue << items;

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void cancelCurrentBatch() {
            m_QueueMutex.lock();
            {
                qDeleteAll(m_Queue);
                m_Queue.clear();
            }
            m_QueueMutex.unlock();

            notifyQueueIsEmpty();
        }

        bool hasPendingJobs() {
            QMutexLocker locker(&m_QueueMutex);
            bool isEmpty = m_Queue.isEmpty();
            return !isEmpty;
        }

        bool isCancelled() const { return m_Cancel; }
        bool isRunning() const { return m_IsRunning; }

        void doWork() {
            if (initWorker()) {
                m_IsRunning = true;
                runWorkerLoop();
                m_IsRunning = false;
            } else {
                m_Cancel = true;
            }

            notifyStopped();
        }

        void stopWorking() {
            m_Cancel = true;

            m_QueueMutex.lock();
            {
                qDeleteAll(m_Queue);
                m_Queue.clear();

                T *stopItem = NULL;

                bool wasEmpty = m_Queue.isEmpty();
                m_Queue.append(stopItem);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

    protected:
        virtual bool initWorker() = 0;
        virtual bool processOneItem(T *item) = 0;
        virtual void notifyQueueIsEmpty() = 0;
        virtual void notifyStopped() = 0;

        void runWorkerLoop() {
            for (;;) {
                if (m_Cancel) {
                    LOG_INFO << "Cancelled. Exiting...";
                    break;
                }

                bool noMoreItems = false;

                m_QueueMutex.lock();

                while (m_Queue.isEmpty()) {
                    bool waitResult = m_WaitAnyItem.wait(&m_QueueMutex);
                    if (!waitResult) {
                        LOG_WARNING << "Waiting failed for new items";
                    }
                }

                T *item = m_Queue.first();
                m_Queue.removeFirst();

                noMoreItems = m_Queue.isEmpty();

                m_QueueMutex.unlock();

                if (item == NULL) { break; }

                bool canDelete = true;

                try {
                    canDelete = processOneItem(item);
                }
                catch (...) {
                    LOG_WARNING << "Exception while processing item!";
                }

                if (canDelete) {
                    QObject *object = dynamic_cast<QObject*>(item);
                    if (object != NULL) {
                        object->deleteLater();
                    } else {
                        delete item;
                    }
                }

                if (noMoreItems) {
                    notifyQueueIsEmpty();
                }
            }
        }

    private:
        QWaitCondition m_WaitAnyItem;
        QMutex m_QueueMutex;
        QVector<T *> m_Queue;
        volatile bool m_Cancel;
        volatile bool m_IsRunning;
    };
}

#endif // ITEMPROCESSINGWORKER_H
