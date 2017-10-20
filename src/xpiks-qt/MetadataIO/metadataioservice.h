/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAIOSERVICE_H
#define METADATAIOSERVICE_H

#include <QObject>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Suggestion/locallibraryquery.h"
#include "artworkssnapshot.h"
#include "../Common/delayedactionentity.h"

namespace Models {
    class ArtworkMetadata;
}
namespace MetadataIO {
    class MetadataIOWorker;

    class MetadataIOService:
            public QObject,
            public Common::BaseEntity,
            public Common::DelayedActionEntity
    {
        Q_OBJECT
    public:
        MetadataIOService(QObject *parent = nullptr);

    public:
        void startService();
        void stopService();

    public:
        void cancelBatch(quint32 batchID) const;

    public:
        void readArtwork(Models::ArtworkMetadata *metadata) const;
        void writeArtwork(Models::ArtworkMetadata *metadata);
        quint32 readArtworks(const ArtworksSnapshot &snapshot) const;
        void writeArtworks(const WeakArtworksSnapshot &artworks) const;
        void addArtworks(const WeakArtworksSnapshot &artworks) const;

    public:
        void searchArtworks(Suggestion::LocalLibraryQuery *query);

    signals:
        void cacheSyncRequest();

#ifdef INTEGRATION_TESTS
    public:
        MetadataIOWorker *getWorker() { return m_MetadataIOWorker; }
#endif

    private slots:
        void onCacheSyncRequest();
        void workerFinished();

        // DelayedActionEntity implementation
    protected:
        virtual void doKillTimer(int timerId) override { this->killTimer(timerId); }
        virtual int doStartTimer(int interval, Qt::TimerType timerType) override { return this->startTimer(interval, timerType); }
        virtual void doOnTimer() override;
        virtual void timerEvent(QTimerEvent *event) override { onQtTimer(event); }

    private:
        MetadataIOWorker *m_MetadataIOWorker;
        bool m_IsStopped;
    };
}

#endif // METADATAIOSERVICE_H
