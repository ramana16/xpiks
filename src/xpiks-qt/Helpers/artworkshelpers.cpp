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

}
