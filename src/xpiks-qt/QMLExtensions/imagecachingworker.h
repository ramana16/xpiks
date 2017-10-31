/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMAGECACHINGWORKER_H
#define IMAGECACHINGWORKER_H

#include "../Common/itemprocessingworker.h"
#include <QString>
#include "imagecacherequest.h"
#include "cachedimage.h"
#include "dbimagecacheindex.h"

namespace Helpers {
    class AsyncCoordinator;
    class DatabaseManager;
}

namespace QMLExtensions {
    class ImageCachingWorker : public QObject, public Common::ItemProcessingWorker<ImageCacheRequest>
    {
        Q_OBJECT
    public:
        ImageCachingWorker(Helpers::AsyncCoordinator *initCoordinator, Helpers::DatabaseManager *dbManager, QObject *parent=0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItemEx(std::shared_ptr<ImageCacheRequest> &item, batch_id_t batchID, Common::flag_t flags) override;
        virtual void processOneItem(std::shared_ptr<ImageCacheRequest> &item) override;

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override;

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    public:
        void setScale(qreal scale) { m_Scale = scale; }
        bool tryGetCachedImage(const QString &key, const QSize &requestedSize,
                               QString &cached, bool &needsUpdate);
        bool upgradeCacheStorage();

    private:
        void saveIndex();
        bool isProcessed(std::shared_ptr<ImageCacheRequest> &item);

    private:
        Helpers::AsyncCoordinator *m_InitCoordinator;
        volatile int m_ProcessedItemsCount;
        DbImageCacheIndex m_Cache;
        qreal m_Scale;
        QString m_ImagesCacheDir;
    };
}

#endif // IMAGECACHINGWORKER_H
