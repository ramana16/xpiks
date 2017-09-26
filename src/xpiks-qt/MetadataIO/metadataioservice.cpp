/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadataioservice.h"
#include <QThread>
#include <QTimerEvent>
#include "metadataioworker.h"
#include "metadataiotask.h"
#include "../Commands/commandmanager.h"
#include "../Helpers/database.h"

#define SAVER_TIMER_TIMEOUT 3000
#define SAVER_TIMER_MAX_RESTARTS 5

namespace MetadataIO {
    MetadataIOService::MetadataIOService(QObject *parent):
        QObject(parent),
        m_LastTimerId(-1),
        m_RestartsCount(0),
        m_MetadataIOWorker(nullptr)
    {
        QObject::connect(this, &MetadataIOService::cacheSyncRequest, this, &MetadataIOService::onCacheSyncRequest);
    }

    void MetadataIOService::startService() {
        Q_ASSERT(m_MetadataIOWorker == nullptr);
        Helpers::DatabaseManager *dbManager = m_CommandManager->getDatabaseManager();
        QMLExtensions::ArtworksUpdateHub *updatesHub = m_CommandManager->getArtworksUpdateHub();

        m_MetadataIOWorker = new MetadataIOWorker(dbManager, updatesHub);

        QThread *thread = new QThread();
        m_MetadataIOWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_MetadataIOWorker, &MetadataIOWorker::process);
        QObject::connect(m_MetadataIOWorker, &MetadataIOWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_MetadataIOWorker, &MetadataIOWorker::stopped, m_MetadataIOWorker, &MetadataIOWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(m_MetadataIOWorker, &MetadataIOWorker::stopped,
                         this, &MetadataIOService::workerFinished);

        thread->start();
    }

    void MetadataIOService::stopService() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_MetadataIOWorker != nullptr);
        m_MetadataIOWorker->stopWorking();
    }

    void MetadataIOService::cancelBatch(quint32 batchID) const {
        LOG_INFO << batchID;
        Q_ASSERT(m_MetadataIOWorker != nullptr);
        m_MetadataIOWorker->cancelBatch(batchID);
    }

    void MetadataIOService::readArtwork(Models::ArtworkMetadata *metadata) const {
        Q_ASSERT(metadata != nullptr);

        std::shared_ptr<MetadataIOTaskBase> jobItem(new MetadataReadWriteTask(metadata, MetadataReadWriteTask::Read));
        m_MetadataIOWorker->submitItem(jobItem);
    }

    void MetadataIOService::writeArtwork(Models::ArtworkMetadata *metadata) {
        Q_ASSERT(metadata != nullptr);

        std::shared_ptr<MetadataIOTaskBase> jobItem(new MetadataReadWriteTask(metadata, MetadataReadWriteTask::Write));
        m_MetadataIOWorker->submitItem(jobItem);

        emit cacheSyncRequest();
    }

    quint32 MetadataIOService::readArtworks(const ArtworksSnapshot &snapshot) const {
        LOG_INFO << snapshot.size() << "artwork(s)";
        std::vector<std::shared_ptr<MetadataIOTaskBase> > jobs;
        jobs.reserve(snapshot.size());

        auto &items = snapshot.getRawData();

        for (auto &item: items) {
            Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
            jobs.emplace_back(new MetadataReadWriteTask(artwork, MetadataReadWriteTask::Read));
        }

        MetadataIOWorker::batch_id_t batchID = m_MetadataIOWorker->submitItems(jobs);
        LOG_INFO << "Batch ID is" << batchID;

        return batchID;
    }

    void MetadataIOService::writeArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const {
        LOG_INFO << artworks.size() << "artwork(s)";
        std::vector<std::shared_ptr<MetadataIOTaskBase> > jobs;
        jobs.reserve(artworks.length());

        int size = artworks.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *artwork = artworks.at(i);
            jobs.emplace_back(new MetadataReadWriteTask(artwork, MetadataReadWriteTask::Write));
        }

        jobs.emplace_back(new MetadataCacheSyncTask());

        m_MetadataIOWorker->submitItems(jobs);
    }

    void MetadataIOService::addArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const {
        LOG_INFO << artworks.size() << "artwork(s)";
        std::vector<std::shared_ptr<MetadataIOTaskBase> > jobs;
        jobs.reserve(artworks.length());

        int size = artworks.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *artwork = artworks.at(i);
            jobs.emplace_back(new MetadataReadWriteTask(artwork, MetadataReadWriteTask::Add));
        }

        m_MetadataIOWorker->submitItems(jobs);

        std::shared_ptr<MetadataIOTaskBase> syncJob(new MetadataCacheSyncTask());
        m_MetadataIOWorker->submitItem(syncJob);
    }

    void MetadataIOService::searchArtworks(Suggestion::LocalLibraryQuery *query) {
        LOG_DEBUG << "#";
        Q_ASSERT(query != nullptr);
        std::shared_ptr<MetadataSearchTask> jobItem(new MetadataSearchTask(query));
        m_MetadataIOWorker->submitFirst(jobItem);
    }

    void MetadataIOService::onCacheSyncRequest() {
        LOG_DEBUG << "#";

        if (m_RestartsCount < SAVER_TIMER_MAX_RESTARTS) {
            if (m_LastTimerId != -1) { this->killTimer(m_LastTimerId); }

            m_LastTimerId = this->startTimer(SAVER_TIMER_TIMEOUT, Qt::VeryCoarseTimer);
            m_RestartsCount++;
        } else {
            LOG_INFO << "Maximum backup delays occured, forcing backup";
        }
    }

    void MetadataIOService::workerFinished() {
        LOG_INFO << "#";
    }

    void MetadataIOService::timerEvent(QTimerEvent *event) {
        LOG_DEBUG << "#";
        if ((event != nullptr) && (event->timerId() == m_LastTimerId)) {
            m_RestartsCount = 0;
            m_LastTimerId = -1;

            std::shared_ptr<MetadataIOTaskBase> syncItem(new MetadataCacheSyncTask());
            m_MetadataIOWorker->submitItem(syncItem);
        }

        this->killTimer(event->timerId());
    }
}
