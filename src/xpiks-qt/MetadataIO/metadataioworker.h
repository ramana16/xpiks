/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAIOWORKER_H
#define METADATAIOWORKER_H

#include <QObject>
#include <QSet>
#include "../Common/itemprocessingworker.h"
#include "metadataiotask.h"
#include "metadatacache.h"

namespace Helpers {
    class DatabaseManager;
}

namespace QMLExtensions {
    class ArtworksUpdateHub;
}

namespace MetadataIO {
    class MetadataIOWorker : public QObject, public Common::ItemProcessingWorker<MetadataIOTaskBase>
    {
        Q_OBJECT
    public:
        explicit MetadataIOWorker(Helpers::DatabaseManager *dbManager, QMLExtensions::ArtworksUpdateHub *artworksUpdateHub, QObject *parent = 0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<MetadataIOTaskBase> &item) override;

    private:
        void processReadWriteItem(std::shared_ptr<MetadataReadWriteTask> &item);
        void processSearchItem(std::shared_ptr<MetadataSearchTask> &item);

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override;

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        MetadataCache m_MetadataCache;
        QMLExtensions::ArtworksUpdateHub *m_ArtworksUpdateHub;
        volatile int m_ProcessedItemsCount;
    };
}

#endif // METADATAIOWORKER_H
