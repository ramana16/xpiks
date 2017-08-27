/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
