/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworkshelpers.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "../Models/videoartwork.h"

namespace Helpers {
    void splitImagesVideo(const QVector<Models::ArtworkMetadata *> &artworks, QVector<Models::ArtworkMetadata *> &imageArtworks, QVector<Models::ArtworkMetadata *> &videoArtworks) {
        int size = artworks.size();
        imageArtworks.reserve(size / 2);
        videoArtworks.reserve(size / 2);

        for (auto *artwork: artworks) {
            Q_ASSERT(artwork != nullptr);
            if (artwork == nullptr) { continue; }

            Models::ImageArtwork *imageArtwork = dynamic_cast<Models::ImageArtwork*>(artwork);
            if (imageArtwork != nullptr) {
                imageArtworks.append(artwork);
            } else {
                Models::VideoArtwork *videoArtwork = dynamic_cast<Models::VideoArtwork*>(artwork);
                if (videoArtwork != nullptr) {
                    videoArtworks.append(artwork);
                } else {
                    Q_ASSERT(false);
                    LOG_WARNING << "Unknown type";
                }
            }
        }
    }

    void splitImagesVideo(const std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &rawSnapshot,
                          std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &imagesRawSnapshot,
                          std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &videoRawSnapshot) {
        const size_t size = rawSnapshot.size();
        imagesRawSnapshot.reserve(size / 2);
        videoRawSnapshot.reserve(size / 2);

        for (auto &locker: rawSnapshot) {
            Models::ArtworkMetadata *artwork = locker->getArtworkMetadata();

            Q_ASSERT(artwork != nullptr);
            if (artwork == nullptr) { continue; }

            Models::ImageArtwork *imageArtwork = dynamic_cast<Models::ImageArtwork*>(artwork);
            if (imageArtwork != nullptr) {
                imagesRawSnapshot.push_back(locker);
            } else {
                Models::VideoArtwork *videoArtwork = dynamic_cast<Models::VideoArtwork*>(artwork);
                if (videoArtwork != nullptr) {
                    videoRawSnapshot.push_back(locker);
                } else {
                    Q_ASSERT(false);
                    LOG_WARNING << "Unknown type";
                }
            }
        }
    }

    int retrieveImagesCount(const std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &rawSnapshot) {
        int count = 0;

        for (auto &locker: rawSnapshot) {
            Models::ArtworkMetadata *artwork = locker->getArtworkMetadata();

            Q_ASSERT(artwork != nullptr);
            if (artwork == nullptr) { continue; }

            Models::ImageArtwork *imageArtwork = dynamic_cast<Models::ImageArtwork*>(artwork);
            if (imageArtwork != nullptr) {
                count++;
            }
        }

        return count;
    }

    int retrieveVideosCount(const std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &rawSnapshot) {
        int count = 0;

        for (auto &locker: rawSnapshot) {
            Models::ArtworkMetadata *artwork = locker->getArtworkMetadata();

            Q_ASSERT(artwork != nullptr);
            if (artwork == nullptr) { continue; }

            Models::VideoArtwork *imageArtwork = dynamic_cast<Models::VideoArtwork*>(artwork);
            if (imageArtwork != nullptr) {
                count++;
            }
        }

        return count;
    }

}
