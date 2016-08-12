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
#include <deque>
#include <memory>
#include <vector>
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

        virtual ~ItemProcessingWorker() { }

    public:
        void submitItem(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return;
            }

            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.empty();
                m_Queue.push_back(item);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void submitFirst(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return;
            }

            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.empty();
                m_Queue.push_front(item);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void submitItems(const std::vector<std::shared_ptr<T> > &items) {
            if (m_Cancel) {
                return;
            }

            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.empty();

                size_t size = items.size();
                for (size_t i = 0; i < size; ++i) {
                    auto &item = items.at(i);
                    m_Queue.push_back(item);
                }

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void submitFirst(const std::vector<std::shared_ptr<T> > &items) {
            if (m_Cancel) {
                return;
            }

            m_QueueMutex.lock();
            {
                bool wasEmpty = m_Queue.empty();

                size_t size = items.size();
                for (size_t i = 0; i < size; ++i) {
                    auto &item = items.at(i);
                    m_Queue.push_front(item);
                }

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        void cancelCurrentBatch() {
            m_QueueMutex.lock();
            {
                m_Queue.clear();
            }
            m_QueueMutex.unlock();

            notifyQueueIsEmpty();
        }

        bool hasPendingJobs() {
            QMutexLocker locker(&m_QueueMutex);
            bool isEmpty = m_Queue.empty();
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

            workerStopped();
        }

        void stopWorking(bool immediately=true) {
            m_Cancel = true;

            m_QueueMutex.lock();
            {
                if (immediately) {
                    m_Queue.clear();
                }

                m_Queue.emplace_back(std::shared_ptr<T>());
                m_WaitAnyItem.wakeOne();
            }
            m_QueueMutex.unlock();
        }

    protected:
        virtual bool initWorker() = 0;
        virtual void processOneItem(std::shared_ptr<T> &item) = 0;
        virtual void notifyQueueIsEmpty() = 0;
        virtual void workerStopped() = 0;

        void runWorkerLoop() {
            for (;;) {
                if (m_Cancel) {
                    LOG_INFO << "Cancelled. Exiting...";
                    break;
                }

                bool noMoreItems = false;

                m_QueueMutex.lock();

                while (m_Queue.empty()) {
                    bool waitResult = m_WaitAnyItem.wait(&m_QueueMutex);
                    if (!waitResult) {
                        LOG_WARNING << "Waiting failed for new items";
                    }
                }

                std::shared_ptr<T> item = m_Queue.front();
                m_Queue.pop_front();

                noMoreItems = m_Queue.empty();

                m_QueueMutex.unlock();

                if (item.get() == nullptr) { break; }

                try {
                    processOneItem(item);
                }
                catch (...) {
                    LOG_WARNING << "Exception while processing item!";
                }

                if (noMoreItems) {
                    notifyQueueIsEmpty();
                }
            }
        }

    private:
        QWaitCondition m_WaitAnyItem;
        QMutex m_QueueMutex;
        std::deque<std::shared_ptr<T> > m_Queue;
        volatile bool m_Cancel;
        volatile bool m_IsRunning;
    };
}

#endif // ITEMPROCESSINGWORKER_H
