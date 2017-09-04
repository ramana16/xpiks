/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "videocachingservice.h"
#include <vector>
#include <memory>
#include "videocachingworker.h"
#include "../Models/videoartwork.h"
#include "../QMLExtensions/videocacherequest.h"
#include "../Commands/commandmanager.h"
#include "../Models/switchermodel.h"
#include "../Common/defines.h"

namespace QMLExtensions {
    VideoCachingService::VideoCachingService(QObject *parent) :
        QObject(parent),
        Common::BaseEntity(),
        m_CachingWorker(nullptr),
        m_IsCancelled(false)
    {
    }

    void VideoCachingService::startService() {
        Helpers::DatabaseManager *dbManager = m_CommandManager->getDatabaseManager();

        m_CachingWorker = new VideoCachingWorker(dbManager);
        m_CachingWorker->setCommandManager(m_CommandManager);

        QThread *thread = new QThread();
        m_CachingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_CachingWorker, SLOT(process()));
        QObject::connect(m_CachingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_CachingWorker, SIGNAL(stopped()), m_CachingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        LOG_DEBUG << "starting low priority thread...";
        thread->start(QThread::LowPriority);
    }

    void VideoCachingService::stopService() {
        LOG_DEBUG << "#";

        if (m_CachingWorker != NULL) {
            m_IsCancelled = true;
            m_CachingWorker->stopWorking();
        } else {
            LOG_WARNING << "Caching Worker was NULL";
        }
    }

    void VideoCachingService::generateThumbnails(const MetadataIO::ArtworksSnapshot &snapshot) {
        Q_ASSERT(m_CachingWorker != nullptr);
        LOG_INFO << snapshot.size() << "artworks";

#ifndef INTEGRATION_TESTS
        Models::SwitcherModel *switcher = m_CommandManager->getSwitcherModel();
        const bool goodQualityAllowed = switcher->getGoodQualityVideoPreviews();
#else
        const bool goodQualityAllowed = false;
#endif

        const size_t size = snapshot.size();
        std::vector<std::shared_ptr<VideoCacheRequest> > requests;
        requests.reserve(size);

        for (size_t i = 0, j = 0; i < size; i++) {
            auto *artwork = snapshot.get(i);
            Models::VideoArtwork *videoArtwork = dynamic_cast<Models::VideoArtwork *>(artwork);
            if (videoArtwork != nullptr) {
                const bool withDelay = j % 2 == 0;
                const bool quickThumbnail = true, dontRecache = false;
                requests.emplace_back(new VideoCacheRequest(videoArtwork,
                                                            dontRecache,
                                                            quickThumbnail,
                                                            withDelay,
                                                            goodQualityAllowed));
                j++;
            }
        }

        m_CachingWorker->submitItems(requests);
        m_CachingWorker->submitSaveIndexItem();
    }
}
