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
#include <tuple>
#include <algorithm>
#include "../Common/flags.h"
#include "../Common/defines.h"
#include "../Helpers/threadhelpers.h"

namespace Common {
    template<typename T>
    class ItemProcessingWorker
    {
    public:
        typedef quint32 batch_id_t;
        typedef std::tuple<std::shared_ptr<T>, Common::flag_t, batch_id_t> ItemType;

    public:
        ItemProcessingWorker(int delayPeriod = 0xffffffff):
            m_BatchID(1),
            m_DelayPeriod(delayPeriod),
            m_Cancel(false),
            m_IsRunning(false)
        { }

        virtual ~ItemProcessingWorker() { }

    private:
        enum WorkerFlags {
            FlagIsSeparator = 1 << 0,
            FlagIsStopper = 1 << 1,
            FlagIsWithDelay = 1 << 2
        };

    protected:
        inline bool getIsSeparatorFlag(Common::flag_t flags) const { return Common::HasFlag(flags, FlagIsSeparator); }
        inline bool getIsStopperFlag(Common::flag_t flags) const { return Common::HasFlag(flags, FlagIsStopper); }
        inline bool getWithDelayFlag(Common::flag_t flags) const { return Common::HasFlag(flags, FlagIsWithDelay); }

    public:
        void submitSeparator() {
            if (m_Cancel) { return; }

            m_QueueMutex.lock();
            {
                Common::flag_t flags = 0;
                Common::SetFlag(flags, FlagIsSeparator);

                bool wasEmpty = m_Queue.empty();
                m_Queue.emplace_back(std::shared_ptr<T>(), flags, INVALID_BATCH_ID);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();
        }

        batch_id_t submitItem(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return INVALID_BATCH_ID;
            }

            batch_id_t batchID;
            Common::flag_t flags = 0;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();
                m_Queue.emplace_back(item, flags, batchID);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        batch_id_t submitFirst(const std::shared_ptr<T> &item) {
            if (m_Cancel) {
                return INVALID_BATCH_ID;
            }

            batch_id_t batchID;
            Common::flag_t flags = 0;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();
                m_Queue.emplace_front(item, flags, batchID);

                if (wasEmpty) {
                    m_WaitAnyItem.wakeOne();
                }
            }
            m_QueueMutex.unlock();

            return batchID;
        }

        batch_id_t submitItems(const std::vector<std::shared_ptr<T> > &items) {
            if (m_Cancel) {
                return INVALID_BATCH_ID;
            }

            batch_id_t batchID;
            Common::flag_t commonFlags = 0;
            m_QueueMutex.lock();
            {
                batchID = getNextBatchID();
                bool wasEmpty = m_Queue.empty();

                const size_t size = items.size();
                for (size_t i = 0; i < size; ++i) {
                    auto &item = items.at(i);

                    Common::flag_t flags = commonFlags;
                    if (i % m_DelayPeriod == 0) { Common::SetFlag(flags, FlagIsWithDelay); }

                    m_Queue.emplace_back(item, flags, batchID);
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
            if (batchID == INVALID_BATCH_ID) { return; }

            bool isEmpty = false;
            m_QueueMutex.lock();
            {
                m_Queue.erase(std::remove_if(m_Queue.begin(), m_Queue.end(),
                                             [&batchID](const ItemType &item) {
                    return std::get<2>(item) == batchID;
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

                Common::flag_t flags = 0;
                Common::SetFlag(flags, FlagIsStopper);

                m_Queue.emplace_back(std::shared_ptr<T>(), flags, INVALID_BATCH_ID);
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

        virtual void processOneItemEx(std::shared_ptr<T> &item, batch_id_t batchID, Common::flag_t flags) {
            Q_UNUSED(flags);
            Q_UNUSED(batchID);
            processOneItem(item);
        }

        void runWorkerLoop() {
            m_IdleEvent.set();

            for (;;) {
                if (m_Cancel) {
                    LOG_INFO << "Cancelled. Exiting...";
                    break;
                }

                bool noMoreItems = false;
                std::shared_ptr<T> item;
                Common::flag_t flags = 0;
                batch_id_t batchID = INVALID_BATCH_ID;

                m_QueueMutex.lock();
                {
                    while (m_Queue.empty()) {
                        bool waitResult = m_WaitAnyItem.wait(&m_QueueMutex);
                        if (!waitResult) {
                            LOG_WARNING << "Waiting failed for new items";
                        }
                    }

                    auto &nextItem = m_Queue.front();
                    item = std::get<0>(nextItem);
                    flags = std::get<1>(nextItem);
                    batchID = std::get<2>(nextItem);
                    m_Queue.pop_front();

                    noMoreItems = m_Queue.empty();
                }
                m_QueueMutex.unlock();

                if ((item.get() == nullptr) && getIsStopperFlag(flags)) { break; }

                m_IdleEvent.reset();
                {
                    try {
                        processOneItemEx(item, batchID, flags);
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
        std::deque<ItemType> m_Queue;
        batch_id_t m_BatchID;
        unsigned int m_DelayPeriod;
        volatile bool m_Cancel;
        volatile bool m_IsRunning;
    };
}

#endif // ITEMPROCESSINGWORKER_H
