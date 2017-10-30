/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef VIDEOCACHINGSERVICE_H
#define VIDEOCACHINGSERVICE_H

#include <QObject>
#include <QVector>
#include <memory>
#include <vector>
#include "../Common/baseentity.h"

namespace Models {
    class ArtworkMetadata;
    class ArtworkMetadataLocker;
}

namespace MetadataIO {
    class ArtworksSnapshot;
}

namespace QMLExtensions {
    class VideoCachingWorker;

    class VideoCachingService : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        explicit VideoCachingService(QObject *parent = 0);

    public:
        void startService();
        void stopService();

    public:
        void generateThumbnails(const MetadataIO::ArtworksSnapshot &snapshot);
        void waitWorkerIdle();

    private:
        VideoCachingWorker *m_CachingWorker;
        volatile bool m_IsCancelled;
    };
}

#endif // VIDEOCACHINGSERVICE_H
