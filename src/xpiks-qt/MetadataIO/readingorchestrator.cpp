/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "readingorchestrator.h"
#include <QThread>
#include <QVector>
#include <QMutexLocker>
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "exiv2readingworker.h"

#if defined(TRAVIS_CI)
#define MIN_SPLIT_COUNT 100
#else
#ifdef QT_DEBUG
#define MIN_SPLIT_COUNT 1
#else
#define MIN_SPLIT_COUNT 15
#endif
#endif

#define MAX_READING_THREADS 10
#define MIN_READING_THREADS 1

namespace MetadataIO {
    ReadingOrchestrator::ReadingOrchestrator(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                             const QVector<QPair<int, int> > &rangesToUpdate,
                                             QObject *parent) :
        QObject(parent),
        m_ItemsToRead(itemsToRead),
        m_RangesToUpdate(rangesToUpdate),
        m_ThreadsCount(MIN_READING_THREADS),
        m_FinishedCount(0),
        m_AnyError(false)
    {
        int size = itemsToRead.size();
        if (size >= MIN_SPLIT_COUNT) {
#ifdef QT_DEBUG
            m_ThreadsCount = size;
#else
            int idealThreadCount = qMin(qMax(QThread::idealThreadCount(), MIN_READING_THREADS), MAX_READING_THREADS);
            m_ThreadsCount = qMin(size, idealThreadCount);
#endif

            m_ThreadsCount = Helpers::splitIntoChunks<Models::ArtworkMetadata*>(itemsToRead, m_ThreadsCount, m_SlicedItemsToRead);
        } else {
            m_SlicedItemsToRead.push_back(itemsToRead);
        }

        LOG_INFO << "Using" << m_ThreadsCount << "threads for" << size << "items to read";
    }

    ReadingOrchestrator::~ReadingOrchestrator() {
        LOG_DEBUG << "destroyed";
    }

    void ReadingOrchestrator::startReading() {
        LOG_DEBUG << "#";

        int size = m_SlicedItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            const QVector<Models::ArtworkMetadata *> &itemsToRead = m_SlicedItemsToRead.at(i);

            Exiv2ReadingWorker *worker = new Exiv2ReadingWorker(i, itemsToRead);

            QThread *thread = new QThread();
            worker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, worker, &Exiv2ReadingWorker::process);
            QObject::connect(worker, &Exiv2ReadingWorker::stopped, thread, &QThread::quit);

            QObject::connect(worker, &Exiv2ReadingWorker::stopped, worker, &Exiv2ReadingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            QObject::connect(worker, &Exiv2ReadingWorker::finished, this, &ReadingOrchestrator::onWorkerFinished);

            thread->start();

            LOG_INFO << "Started worker" << i;
        }

        Q_ASSERT(m_SlicedItemsToRead.size() == m_ThreadsCount);

        emit allStarted();
    }

    void ReadingOrchestrator::dismiss() {
        this->deleteLater();
    }

    void ReadingOrchestrator::onWorkerFinished(bool anyError) {
        Exiv2ReadingWorker *worker = qobject_cast<Exiv2ReadingWorker *>(sender());
        Q_ASSERT(worker != NULL);

        LOG_INTEGR_TESTS_OR_DEBUG << "#" << worker->getWorkerIndex() << "[" << m_FinishedCount << "out of" << m_ThreadsCount << "] anyError:" << anyError;

        {
            QMutexLocker locker(&m_ImportMutex);
            m_ImportResult.unite(worker->getImportResult());
            m_AnyError = m_AnyError || anyError;
        }

        worker->dismiss();

        if (m_FinishedCount.fetchAndAddOrdered(1) == (m_ThreadsCount - 1)) {
            LOG_DEBUG << "Last worker finished";
            emit allFinished(!m_AnyError);
        }
    }
}
