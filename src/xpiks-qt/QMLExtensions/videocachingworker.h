/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef VIDEOCACHINGWORKER_H
#define VIDEOCACHINGWORKER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QSet>
#include <vector>
#include "../Common/itemprocessingworker.h"
#include "../Common/baseentity.h"
#include "videocacherequest.h"
#include "cachedvideo.h"
#include "dbvideocacheindex.h"

namespace QMLExtensions {
    class VideoCachingWorker : public QObject, public Common::BaseEntity, public Common::ItemProcessingWorker<VideoCacheRequest>
    {
        Q_OBJECT
    public:
        explicit VideoCachingWorker(Helpers::DatabaseManager *dbManager, QObject *parent = 0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItemEx(Common::flag_t flags, std::shared_ptr<VideoCacheRequest> &item) override;
        virtual void processOneItem(std::shared_ptr<VideoCacheRequest> &item) override;

    private:
        bool createThumbnail(std::shared_ptr<VideoCacheRequest> &item, std::vector<uint8_t> &buffer, int &width, int &height);

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
        bool tryGetVideoThumbnail(const QString &key, QString &cachedPath, bool &needsUpdate);

    private:
        bool saveThumbnail(QImage &image, const QString &originalPath, bool isQuickThumbnail, QString &thumbnailPath);
        void cacheImage(const QString &thumbnailPath);
        void applyThumbnail(std::shared_ptr<VideoCacheRequest> &item, const QString &thumbnailPath);
        void saveIndex();
        bool checkLockedIO(std::shared_ptr<VideoCacheRequest> &item);
        bool checkProcessed(std::shared_ptr<VideoCacheRequest> &item);

    private:
        volatile int m_ProcessedItemsCount;
        qreal m_Scale;
        QString m_VideosCacheDir;
        DbVideoCacheIndex m_Cache;
        QSet<int> m_RolesToUpdate;
    };
}

#endif // VIDEOCACHINGWORKER_H
