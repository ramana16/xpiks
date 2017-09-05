/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef READERWRITERQUEUE_H
#define READERWRITERQUEUE_H

#include <QMutex>
#include <vector>
#include <memory>

namespace Common {
    // queue optimized for "many writers - 1 reader" case
    template<typename T>
    class ReaderWriterQueue
    {
    public:
        ReaderWriterQueue() {}

    public:
        void push(const std::shared_ptr<T> &item) {
            QMutexLocker writeLocker(&m_WriteMutex);
            Q_UNUSED(writeLocker);

            m_WriteQueue.push_back(item);
        }

        void reservePush(size_t n) {
            QMutexLocker writeLocker(&m_WriteMutex);
            Q_UNUSED(writeLocker);

            m_WriteQueue.reserve(n);
        }

        bool popAll(std::vector<std::shared_ptr<T> > &out) {
            bool success = false;

            {
                QMutexLocker readLocker(&m_ReadMutex);
                Q_UNUSED(readLocker);

                {
                    QMutexLocker writeLocker(&m_WriteMutex);
                    Q_UNUSED(writeLocker);

                    rebalanceUnsafe();
                }

                if (!m_ReadQueue.empty()) {
                    m_ReadQueue.swap(out);
                    success = !out.empty();
                }
            }

            return success;
        }

        bool empty() {
            QMutexLocker readLocker(&m_ReadMutex);
            Q_UNUSED(readLocker);

            return m_ReadQueue.empty();
        }

        std::shared_ptr<T> top() {
            std::shared_ptr<T> result;

            {
                QMutexLocker readLocker(&m_ReadMutex);
                Q_UNUSED(readLocker);

                if (m_ReadQueue.empty()) {
                    QMutexLocker writeLocker(&m_WriteMutex);
                    Q_UNUSED(writeLocker);

                    rebalanceUnsafe();
                }

                if (!m_ReadQueue.empty()) {
                    result = m_ReadQueue.back();
                }
            }

            return result;
        }

        bool pop() {
            bool success = false;

            {
                QMutexLocker readLocker(&m_ReadMutex);
                Q_UNUSED(readLocker);

                if (m_ReadQueue.empty()) {
                    QMutexLocker writeLocker(&m_WriteMutex);
                    Q_UNUSED(writeLocker);

                    rebalanceUnsafe();
                }

                if (!m_ReadQueue.empty()) {
                    m_ReadQueue.pop_back();
                    success = true;
                }
            }

            return success;
        }

        void rebalance() {
            QMutexLocker readLocker(&m_ReadMutex);
            Q_UNUSED(readLocker);

            {
                QMutexLocker writeLocker(&m_WriteMutex);
                Q_UNUSED(writeLocker);

                rebalanceUnsafe();
            }
        }

        void clear() {
            QMutexLocker readLocker(&m_ReadMutex);
            Q_UNUSED(readLocker);

            {
                QMutexLocker writeLocker(&m_WriteMutex);
                Q_UNUSED(writeLocker);

                m_ReadQueue.clear();
                m_WriteQueue.clear();
            }
        }

    private:
        void rebalanceUnsafe() {
            Q_ASSERT(m_ReadQueue.empty());

            const size_t size = m_WriteQueue.size();
            if (size == 0) { return; }

            m_ReadQueue.reserve(size);

            // reverse in order to use pop_back() in read
            for (size_t i = size; i >= 1; i--) {
                m_ReadQueue.push_back(m_WriteQueue[i - 1]);
            }

            m_WriteQueue.clear();
        }

    private:
        QMutex m_ReadMutex;
        QMutex m_WriteMutex;
        std::vector<std::shared_ptr<T> > m_ReadQueue;
        std::vector<std::shared_ptr<T> > m_WriteQueue;
    };
}

#endif // READERWRITERQUEUE_H
