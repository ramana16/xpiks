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
#include <QHash>
#include <QDateTime>
#include <QSize>
#include <QReadWriteLock>
#include "imagecacherequest.h"

namespace Helpers {
    class AsyncCoordinator;
}

namespace QMLExtensions {
    struct CachedImage {
        QDateTime m_LastModified;
        QString m_Filename;
        QSize m_Size;
        quint64 m_RequestsServed;
        // reserved for future demands
        QHash<qint32, QByteArray> m_AdditionalData;
    };

    QDataStream &operator<<(QDataStream &out, const CachedImage &v);
    QDataStream &operator>>(QDataStream &in, CachedImage &v);

    class ImageCachingWorker : public QObject, public Common::ItemProcessingWorker<ImageCacheRequest>
    {
        Q_OBJECT
    public:
        ImageCachingWorker(Helpers::AsyncCoordinator *initCoordinator, QObject *parent=0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<ImageCacheRequest> &item) override;

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { saveIndex(); emit stopped(); }

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
        void splitToCachedAndNot(const std::vector<std::shared_ptr<ImageCacheRequest> > allRequests,
                                 std::vector<std::shared_ptr<ImageCacheRequest> > &unknownRequests,
                                 std::vector<std::shared_ptr<ImageCacheRequest> > &knownRequests);

    private:
        void readIndex();
        void saveIndex();
        bool isProcessed(std::shared_ptr<ImageCacheRequest> &item);

    private:
        Helpers::AsyncCoordinator *m_InitCoordinator;
        volatile int m_ProcessedItemsCount;
        qreal m_Scale;
        QString m_ImagesCacheDir;
        QString m_IndexFilepath;
        QReadWriteLock m_CacheLock;
        QHash<QString, CachedImage> m_CacheIndex;
    };
}

#endif // IMAGECACHINGWORKER_H
