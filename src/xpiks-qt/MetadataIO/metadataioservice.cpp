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
#include "metadataioworker.h"
#include "metadataiotask.h"
#include "../Commands/commandmanager.h"
#include "../Helpers/database.h"

namespace MetadataIO {
    MetadataIOService::MetadataIOService(QObject *parent):
        QObject(parent),
        m_MetadataIOWorker(nullptr)
    {
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

    void MetadataIOService::writeArtwork(Models::ArtworkMetadata *metadata) const {
        Q_ASSERT(metadata != nullptr);

        std::shared_ptr<MetadataIOTaskBase> jobItem(new MetadataReadWriteTask(metadata, MetadataReadWriteTask::Write));
        m_MetadataIOWorker->submitItem(jobItem);
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

        QMLExtensions::ArtworksUpdateHub *updateHub = m_CommandManager->getArtworksUpdateHub();
        std::shared_ptr<MetadataIOTaskBase> updateHubTask(new UpdateHubHighFrequencyModeTask(updateHub));
        m_MetadataIOWorker->submitItem(updateHubTask);

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

    void MetadataIOService::workerFinished() {
        LOG_INFO << "#";
    }
}
