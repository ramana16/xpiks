/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "imagecachingworker.h"
#include <QDir>
#include <QFile>
#include <QImage>
#include <QString>
#include <QFileInfo>
#include <QByteArray>
#include <QDataStream>
#include <QReadLocker>
#include <QWriteLocker>
#include <QCryptographicHash>
#include "../Common/defines.h"
#include "../Helpers/constants.h"
#include "imagecacherequest.h"
#include "../Helpers/asynccoordinator.h"
#include "dbimagecacheindex.h"

#define IMAGE_CACHING_WORKER_SLEEP_DELAY 500
#define IMAGES_INDEX_BACKUP_STEP 50
#define PREVIEW_JPG_QUALITY 70

namespace QMLExtensions {
    QString getImagePathHash(const QString &path) {
        return QString::fromLatin1(QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Sha256).toHex());
    }

    ImageCachingWorker::ImageCachingWorker(Helpers::AsyncCoordinator *initCoordinator, Helpers::DatabaseManager *dbManager, QObject *parent):
        QObject(parent),
        m_InitCoordinator(initCoordinator),
        m_ProcessedItemsCount(0),
        m_Cache(dbManager),
        m_Scale(1.0)
    {
    }

    bool ImageCachingWorker::initWorker() {
        LOG_DEBUG << "#";

        Helpers::AsyncCoordinatorUnlocker unlocker(m_InitCoordinator);
        Q_UNUSED(unlocker);

        m_ProcessedItemsCount = 0;
        QString appDataPath = XPIKS_USERDATA_PATH;

        if (!appDataPath.isEmpty()) {
            m_ImagesCacheDir = QDir::cleanPath(appDataPath + QDir::separator() + Constants::IMAGES_CACHE_DIR);

            QDir imagesCacheDir(m_ImagesCacheDir);
            if (!imagesCacheDir.exists()) {
                LOG_INFO << "Creating cache dir" << m_ImagesCacheDir;
                QDir().mkpath(m_ImagesCacheDir);
            }
        } else {
            m_ImagesCacheDir = QDir::currentPath();
        }

        LOG_INFO << "Using" << m_ImagesCacheDir << "for images cache";

        m_Cache.initialize();

        return true;
    }

    void ImageCachingWorker::processOneItem(std::shared_ptr<ImageCacheRequest> &item) {
        if (isProcessed(item)) {
            LOG_FOR_DEBUG << item->getFilepath() << "is processed";
            return;
        }
        if (isSeparator(item)) { saveIndex(); return; }

        const QString &originalPath = item->getFilepath();
        QSize requestedSize = item->getRequestedSize();

        LOG_INFO << (item->getNeedRecache() ? "Recaching" : "Caching") << originalPath << "with size" << requestedSize;

        if (!requestedSize.isValid()) {
            LOG_WARNING << "Invalid requestedSize for" << originalPath;
            requestedSize.setHeight(DEFAULT_THUMB_HEIGHT * m_Scale);
            requestedSize.setWidth(DEFAULT_THUMB_WIDTH * m_Scale);
        }

        const bool isInResources = originalPath.startsWith(":/");

        QImage img(originalPath);
        QImage resizedImage = img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QFileInfo fi(originalPath);
        const QString suffix = isInResources ? "jpg" : fi.suffix();
        QString pathHash = getImagePathHash(originalPath) + "." + suffix;
        QString cachedFilepath = QDir::cleanPath(m_ImagesCacheDir + QDir::separator() + pathHash);

        if (resizedImage.save(cachedFilepath, nullptr, PREVIEW_JPG_QUALITY)) {
            CachedImage cachedImage;
            cachedImage.m_Filename = pathHash;
            cachedImage.m_LastModified = isInResources ? QDateTime::currentDateTime() : fi.lastModified();
            cachedImage.m_Size = requestedSize;

            m_Cache.update(originalPath, cachedImage);

            m_ProcessedItemsCount++;
        } else {
            LOG_WARNING << "Failed to save image. Path:" << cachedFilepath << "size" << requestedSize;
        }

        if (m_ProcessedItemsCount % IMAGES_INDEX_BACKUP_STEP == 0) {
            saveIndex();
        }

        if (item->getWithDelay()) {
            // force context switch for more imporant tasks
            QThread::msleep(IMAGE_CACHING_WORKER_SLEEP_DELAY);
        }
    }

    void ImageCachingWorker::workerStopped() {
        LOG_DEBUG << "#";
        m_Cache.finalize();
        emit stopped();
    }

    bool ImageCachingWorker::tryGetCachedImage(const QString &key, const QSize &requestedSize,
                                               QString &cachedPath, bool &needsUpdate) {
        bool found = false;
        CachedImage cachedImage;

        if (m_Cache.tryGet(key, cachedImage)) {
            QString cachedValue = QDir::cleanPath(m_ImagesCacheDir + QDir::separator() + cachedImage.m_Filename);

            QFileInfo fi(cachedValue);

            if (fi.exists()) {
                cachedImage.m_RequestsServed++;
                cachedPath = cachedValue;
                const bool isInResources = key.startsWith(":/");
                const bool isOutdated = (!isInResources) && (QFileInfo(key).lastModified() > cachedImage.m_LastModified);
                needsUpdate = isOutdated || (cachedImage.m_Size != requestedSize);

                found = true;
            }
        }

        return found;
    }

    void ImageCachingWorker::submitSaveIndexItem() {
        std::shared_ptr<ImageCacheRequest> separatorItem(new ImageCacheRequest("", QSize(), true, false));
        this->submitItem(separatorItem);
    }

    QString getOldCacheFilepath() {
        QString appDataPath = XPIKS_USERDATA_PATH;
        QString indexFilepath;

        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            indexFilepath = appDataDir.filePath(Constants::IMAGES_CACHE_INDEX);
        } else {
            indexFilepath = Constants::IMAGES_CACHE_INDEX;
        }

        return indexFilepath;
    }

    bool ImageCachingWorker::upgradeCacheStorage() {
        bool migrated = false;
        QString indexFilepath = getOldCacheFilepath();
        LOG_INFO << "Trying to load old cache index from" << indexFilepath;

        QHash<QString, CachedImage> oldCache;

        QFile file(indexFilepath);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);   // read the data
            in >> oldCache;
            file.close();

            LOG_INFO << "Read" << oldCache.size() << "items from the old cache index";

            m_Cache.importCache(oldCache);
            migrated = true;

            if (file.rename(indexFilepath + ".backup")) {
                LOG_INFO << "Old cache index has been discarded";
            } else {
                LOG_WARNING << "Failed to discard old cache index";
            }
        } else {
            LOG_INFO << "Cannot open old cache index";
        }

        return migrated;
    }

    void ImageCachingWorker::saveIndex() {
        m_Cache.sync();
    }

    bool ImageCachingWorker::isProcessed(std::shared_ptr<ImageCacheRequest> &item) {
        if (item->getNeedRecache()) { return false; }

        const QString &originalPath = item->getFilepath();
        const QSize &requestedSize = item->getRequestedSize();

        bool isAlreadyProcessed = false;

        QString cachedPath;
        bool needsUpdate = false;
        if (this->tryGetCachedImage(originalPath, requestedSize, cachedPath, needsUpdate)) {
            isAlreadyProcessed = !needsUpdate;
        }

        return isAlreadyProcessed;
    }

    bool ImageCachingWorker::isSeparator(const std::shared_ptr<ImageCacheRequest> &item) {
        bool result = item->getFilepath().isEmpty() && item->getNeedRecache() && (!item->getWithDelay());
        return result;
    }
}
