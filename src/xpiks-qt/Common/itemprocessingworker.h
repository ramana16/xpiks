/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ITEMPROCESSINGWORKER_H
#define ITEMPROCESSINGWORKER_H

#include <QWaitCondition>
#include <QMutex>
#include <deque>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include "../Common/defines.h"
#include "../Helpers/threadhelpers.h"

namespace Common {
    template<typename T>
    class ItemProcessingWorker
    {
    public:
        typedef quint32 batch_id_t;

    public:
        ItemProcessingWorker():
            m_BatchID(1),
            m_Cancel(false),
            m_IsRunning(false)
        { }

        virtual ~ItemProcessingWorker() { }

    public:
        batch_id_t submitItem(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return 0;
            }

            batch_id_t batchID;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();
                m_Queue.emplace_back(item, batchID);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        batch_id_t submitFirst(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return 0;
            }

            batch_id_t batchID;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();
                m_Queue.emplace_front(item, batchID);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        batch_id_t submitItems(const std::vector<std::shared_ptr<T> > &items) {
            if (m_Cancel) {
                return 0;
            }

            batch_id_t batchID;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();

                size_t size = items.size();
                for (size_t i = 0; i < size; ++i) {
                    auto &item = items.at(i);
                    m_Queue.emplace_back(item, batchID);
                }

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        batch_id_t submitFirst(const std::vector<std::shared_ptr<T> > &items) {
            if (m_Cancel) {
                return 0;
            }

            batch_id_t batchID;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();

                size_t size = items.size();
                for (size_t i = 0; i < size; ++i) {
                    auto &item = items.at(i);
                    m_Queue.emplace_front(item, batchID);
                }

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        void cancelPendingJobs() {
            m_QueueMutex.lock();
            {
                m_Queue.clear();
            }
            m_QueueMutex.unlock();

            onQueueIsEmpty();
        }

        void cancelBatch(batch_id_t batchID) {
            if (batchID == 0) { return; }

            bool isEmpty = false;
            m_QueueMutex.lock();
            {
                m_Queue.erase(std::remove_if(m_Queue.begin(), m_Queue.end(),
                                             [&batchID](const std::pair<std::shared_ptr<T>, batch_id_t> &item) {
                    return item.second == batchID;
                }),
                              m_Queue.end());

                isEmpty = m_Queue.empty();
            }
            m_QueueMutex.unlock();

            if (isEmpty) {
                onQueueIsEmpty();
            }
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

                m_Queue.emplace_back(std::shared_ptr<T>(), 0);
                m_WaitAnyItem.wakeOne();
            }
            m_QueueMutex.unlock();
        }

        void waitIdle() {
            m_IdleEvent.waitOne();
        }

    protected:
        virtual bool initWorker() = 0;
        virtual void processOneItem(std::shared_ptr<T> &item) = 0;
        virtual void onQueueIsEmpty() = 0;
        virtual void workerStopped() = 0;

        void runWorkerLoop() {
            m_IdleEvent.set();

            for (;;) {
                if (m_Cancel) {
                    LOG_INFO << "Cancelled. Exiting...";
                    break;
                }

                bool noMoreItems = false;
                std::shared_ptr<T> item;

                m_QueueMutex.lock();
                {
                    while (m_Queue.empty()) {
                        bool waitResult = m_WaitAnyItem.wait(&m_QueueMutex);
                        if (!waitResult) {
                            LOG_WARNING << "Waiting failed for new items";
                        }
                    }

                    auto &nextItem = m_Queue.front();
                    item = nextItem.first;
                    m_Queue.pop_front();

                    noMoreItems = m_Queue.empty();
                }
                m_QueueMutex.unlock();

                if (item.get() == nullptr) { break; }

                m_IdleEvent.reset();
                {
                    try {
                        processOneItem(item);
                    }
                    catch (...) {
                        LOG_WARNING << "Exception while processing item!";
                    }
                }
                m_IdleEvent.set();

                if (noMoreItems) {
                    onQueueIsEmpty();
                }
            }
        }

    private:
        inline batch_id_t getNextBatchID() {
            batch_id_t id = m_BatchID++;
            return id;
        }

    private:
        Helpers::ManualResetEvent m_IdleEvent;
        QWaitCondition m_WaitAnyItem;
        QMutex m_QueueMutex;
        std::deque<std::pair<std::shared_ptr<T>, batch_id_t> > m_Queue;
        batch_id_t m_BatchID;
        volatile bool m_Cancel;
        volatile bool m_IsRunning;
    };
}

#endif // ITEMPROCESSINGWORKER_H
