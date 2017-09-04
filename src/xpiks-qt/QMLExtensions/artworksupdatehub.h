/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKSUPDATEHUB_H
#define ARTWORKSUPDATEHUB_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QSet>
#include <vector>
#include <memory>
#include "../Common/baseentity.h"
#include "artworksupdatehub.h"

namespace Models {
    class ArtworkMetadata;
}

namespace QMLExtensions {
    class ArtworkUpdateRequest;

    class ArtworksUpdateHub : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        explicit ArtworksUpdateHub(QObject *parent = 0);

    public:
        void setStandardRoles(const QVector<int> &roles);

    public:
        void updateArtwork(qint64 artworkID, size_t lastKnownIndex, const QSet<int> &rolesToUpdate = QSet<int>());
        void updateArtwork(Models::ArtworkMetadata *artwork);
        void forceUpdate();

#ifdef INTEGRATION_TESTS
    public:
        void clear();
#endif

    signals:
        void updateRequested();

    private slots:
        void onUpdateRequested();
        void onUpdateTimer();

    private:
        QMutex m_Lock;
        QSet<int> m_StandardRoles;
        std::vector<std::shared_ptr<ArtworkUpdateRequest> > m_UpdateRequests;
        QTimer m_UpdateTimer;
        int m_TimerRestartedCount;
    };
}

#endif // ARTWORKSUPDATEHUB_H
