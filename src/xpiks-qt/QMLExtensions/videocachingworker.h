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

#ifndef VIDEOCACHINGWORKER_H
#define VIDEOCACHINGWORKER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QSet>
#include "../Common/itemprocessingworker.h"
#include "videocacherequest.h"
#include "cachedvideo.h"
#include "dbvideocacheindex.h"

namespace QMLExtensions {
    class ImageCachingService;
    class ArtworksUpdateHub;

    class VideoCachingWorker : public QObject, public Common::ItemProcessingWorker<VideoCacheRequest>
    {
        Q_OBJECT
    public:
        explicit VideoCachingWorker(ImageCachingService *imageCachingService, ArtworksUpdateHub *artworksUpdateHub, Helpers::DatabaseManager *dbManager, QObject *parent = 0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<VideoCacheRequest> &item) override;

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
        void submitSaveIndexItem();

    private:
        bool saveThumbnail(QImage &image, const QString &originalPath, bool isSmartThumbnail, QString &thumbnailPath);
        void saveIndex();
        bool isProcessed(std::shared_ptr<VideoCacheRequest> &item);
        bool isSeparator(const std::shared_ptr<VideoCacheRequest> &item);

    private:
        ImageCachingService *m_ImageCachingService;
        ArtworksUpdateHub *m_ArtworksUpdateHub;
        volatile int m_ProcessedItemsCount;
        qreal m_Scale;
        QString m_VideosCacheDir;
        DbVideoCacheIndex m_Cache;
        QSet<int> m_RolesToUpdate;
    };
}

#endif // VIDEOCACHINGWORKER_H
