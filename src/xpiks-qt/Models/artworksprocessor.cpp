/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworksprocessor.h"
#include <QtConcurrent>
#include "../Common/defines.h"

namespace Models {
    void ArtworksProcessor::resetModel() {
        LOG_DEBUG << "#";
        setInProgress(false);
        setIsError(false);
        innerResetModel();
        resetArtworks();
        m_ArtworksCount = 0;
        m_ProcessedArtworksCount = 0;
        updateProgress();
    }

    bool ArtworksProcessor::removeUnavailableItems() {
        LOG_DEBUG << "#";

        auto &artworksListOld = getArtworksSnapshot();
        MetadataIO::ArtworksSnapshot::Container artworksListNew;

        const size_t size = artworksListOld.size();
        for (size_t i = 0; i < size; ++i) {
            auto &item = artworksListOld.at(i);

            if (!item->getArtworkMetadata()->isUnavailable()) {
                artworksListNew.push_back(item);
            }
        }

        bool anyUnavailable = artworksListNew.size() != m_ArtworksSnapshot.size();
        if (anyUnavailable) {
            m_ArtworksSnapshot.set(artworksListNew);

            if (artworksListNew.empty()) {
                emit requestCloseWindow();
            }

            emit itemsNumberChanged();
        }

        return anyUnavailable;
    }

    void ArtworksProcessor::beginProcessing() {
        m_ExistingMaxThreadsNumber = QThreadPool::globalInstance()->maxThreadCount();
        LOG_DEBUG << "Saving pools max threads" << m_ExistingMaxThreadsNumber;
        m_ArtworksCount = (int)m_ArtworksSnapshot.size();
        m_ProcessedArtworksCount = 0;
        updateProgress();
        setInProgress(true);
        emit startedProcessing();
    }

    void ArtworksProcessor::endProcessing() {
        m_ProcessedArtworksCount = 0;
        m_ArtworksCount = 0;
        LOG_DEBUG << "Restoring pool max threads to" << m_ExistingMaxThreadsNumber;
        QThreadPool::globalInstance()->setMaxThreadCount(m_ExistingMaxThreadsNumber);
        setInProgress(false);
        emit finishedProcessing();
    }

    void ArtworksProcessor::endAfterFirstError() {
        setIsError(true);
        m_ProcessedArtworksCount = m_ArtworksCount;
        updateProgress();
        endProcessing();
    }

    void ArtworksProcessor::restrictMaxThreads() {
        LOG_DEBUG << (int)MAX_WORKER_THREADS;
        QThreadPool::globalInstance()->setMaxThreadCount((int)MAX_WORKER_THREADS);
    }
}
