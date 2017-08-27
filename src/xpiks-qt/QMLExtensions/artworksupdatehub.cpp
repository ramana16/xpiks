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

#include "artworksupdatehub.h"
#include "artworkupdaterequest.h"
#include "../Commands/commandmanager.h"
#include "../Models/artitemsmodel.h"
#include "../Models/artworkproxymodel.h"
#include "../Common/defines.h"

#define MAX_NOT_UPDATED_ARTWORKS_TO_HOLD 50
#define MAX_UPDATE_TIMER_DELAYS 5
#define UPDATE_TIMER_DELAY 2000
#define UPDATE_TIMER_HIGH_FREQ_DELAY 500

namespace QMLExtensions {
    ArtworksUpdateHub::ArtworksUpdateHub(QObject *parent) :
        QObject(parent),
        Common::BaseEntity(),
        m_TimerRestartedCount(0),
        m_HighFrequencyMode(false)
    {
        m_UpdateTimer.setSingleShot(true);

        QObject::connect(&m_UpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));
        QObject::connect(this, SIGNAL(updateRequested()), this, SLOT(onUpdateRequested()));

        m_UpdateRequests.reserve(100);
    }

    void ArtworksUpdateHub::setStandardRoles(const QVector<int> &roles) {
        m_StandardRoles = roles.toList().toSet();
    }

    void ArtworksUpdateHub::updateArtwork(qint64 artworkID, size_t lastKnownIndex, const QSet<int> &rolesToUpdate) {
        {
            QMutexLocker locker(&m_Lock);

            std::shared_ptr<ArtworkUpdateRequest> updateRequest(new ArtworkUpdateRequest(artworkID, lastKnownIndex, rolesToUpdate));
            m_UpdateRequests.emplace_back(updateRequest);
        }

        emit updateRequested();
    }

    void ArtworksUpdateHub::updateArtwork(Models::ArtworkMetadata *artwork) {
        Q_ASSERT(artwork != nullptr);
        this->updateArtwork(artwork->getItemID(), artwork->getLastKnownIndex(), m_StandardRoles);
    }

    void ArtworksUpdateHub::forceUpdate() {
        LOG_DEBUG << "#";

        {
            QMutexLocker locker(&m_Lock);
            Q_UNUSED(locker);
            m_TimerRestartedCount += MAX_UPDATE_TIMER_DELAYS;
        }

        emit updateRequested();
    }

    void ArtworksUpdateHub::onUpdateRequested() {
        LOG_DEBUG << "#";

        if (m_TimerRestartedCount < MAX_UPDATE_TIMER_DELAYS) {
            int msec = m_HighFrequencyMode ? UPDATE_TIMER_HIGH_FREQ_DELAY : UPDATE_TIMER_DELAY;
            m_UpdateTimer.start(msec);

            QMutexLocker locker(&m_Lock);
            Q_UNUSED(locker);
            m_TimerRestartedCount++;
        } else {
            LOG_INFO << "Maximum backup delays occured, forcing update";
            Q_ASSERT(m_UpdateTimer.isActive());
        }
    }

    void ArtworksUpdateHub::onUpdateTimer() {
        LOG_DEBUG << "#";
        std::vector<std::shared_ptr<ArtworkUpdateRequest> > requests;

        {
            QMutexLocker locker(&m_Lock);
            Q_UNUSED(locker);
            requests.swap(m_UpdateRequests);
            m_TimerRestartedCount = 0;
        }

        Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
        artItemsModel->processUpdateRequests(requests);

        QSet<qint64> artworkIDsToUpdate;
        QSet<int> commonRoles;

        const size_t size = requests.size();
        std::vector<std::shared_ptr<ArtworkUpdateRequest> > requestsToResubmit;
        requestsToResubmit.reserve(size / 3);

        for (auto &request: requests) {
            if (!request->isCacheMiss()) { continue; }

            // allow cache miss requests to be postponed 1 time
            // in order to gather more of them and process in 1 go later
            if (request->isFirstGeneration()) {
                request->incrementGeneration();
                requestsToResubmit.emplace_back(request);
            } else {
                artworkIDsToUpdate.insert(request->getArtworkID());
                commonRoles.unite(request->getRolesToUpdate());
            }
        }

        LOG_INFO << requestsToResubmit.size() << "requests to resubmit";
        if (!requestsToResubmit.empty()) {
            {
                QMutexLocker locker(&m_Lock);
                m_UpdateRequests.insert(m_UpdateRequests.end(), requestsToResubmit.begin(), requestsToResubmit.end());
            }

            emit updateRequested();
        }

        QVector<int> roles = commonRoles.toList().toVector();
        artItemsModel->updateArtworks(artworkIDsToUpdate, roles);
    }
}
