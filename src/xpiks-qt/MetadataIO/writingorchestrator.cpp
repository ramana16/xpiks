/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "writingorchestrator.h"
#include <QVector>
#include <QThread>
#include "../Helpers/indiceshelper.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "exiv2writingworker.h"

#if defined(TRAVIS_CI)
#define MIN_SPLIT_COUNT 100
#else
#ifdef QT_DEBUG
#define MIN_SPLIT_COUNT 1
#else
#define MIN_SPLIT_COUNT 15
#endif
#endif

#define MAX_WRITING_THREADS 10
#define MIN_WRITING_THREADS 1

namespace MetadataIO {
    WritingOrchestrator::WritingOrchestrator(const QVector<Models::ArtworkMetadata *> &itemsToWrite,
                                             QObject *parent) :
        QObject(parent),
        m_ItemsToWrite(itemsToWrite),
        m_ThreadsCount(MIN_WRITING_THREADS),
        m_FinishedCount(0),
        m_AnyError(false)
    {
        int size = itemsToWrite.size();
        if (size >= MIN_SPLIT_COUNT) {
            int idealThreadCount = qMin(qMax(QThread::idealThreadCount(), MIN_WRITING_THREADS), MAX_WRITING_THREADS);
            m_ThreadsCount = qMin(size, idealThreadCount);

            m_ThreadsCount = Helpers::splitIntoChunks<Models::ArtworkMetadata*>(itemsToWrite, m_ThreadsCount, m_SlicedItemsToWrite);
        } else {
            m_SlicedItemsToWrite.push_back(itemsToWrite);
        }

        LOG_INFO << "Using" << m_ThreadsCount << "threads for" << size << "items to read";
    }

    WritingOrchestrator::~WritingOrchestrator() {
        LOG_DEBUG << "destroyed";
    }

    void WritingOrchestrator::startWriting() {
        LOG_DEBUG << "#";

        int size = m_SlicedItemsToWrite.size();
        for (int i = 0; i < size; ++i) {
            const QVector<Models::ArtworkMetadata *> &itemsToWrite = m_SlicedItemsToWrite.at(i);

            Exiv2WritingWorker *worker = new Exiv2WritingWorker(i, itemsToWrite);

            QThread *thread = new QThread();
            worker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, worker, &Exiv2WritingWorker::process);
            QObject::connect(worker, &Exiv2WritingWorker::stopped, thread, &QThread::quit);

            QObject::connect(worker, &Exiv2WritingWorker::stopped, worker, &Exiv2WritingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            QObject::connect(worker, &Exiv2WritingWorker::finished, this, &WritingOrchestrator::onWorkerFinished);

            thread->start();

            LOG_INFO << "Started worker" << i;
        }

        emit allStarted();
    }

    void WritingOrchestrator::dismiss() {
        this->deleteLater();
    }

    void WritingOrchestrator::onWorkerFinished(bool anyError) {
        Exiv2WritingWorker *worker = qobject_cast<Exiv2WritingWorker *>(sender());
        Q_ASSERT(worker != NULL);

        LOG_INFO << "#" << worker->getWorkerIndex() << "anyError:" << anyError;

        worker->dismiss();

        if (m_FinishedCount.fetchAndAddOrdered(1) == (m_ThreadsCount - 1)) {
            LOG_DEBUG << "Last worker finished";
            emit allFinished(!m_AnyError);
        }
    }
}
