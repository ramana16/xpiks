/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKSHELPERS_H
#define ARTWORKSHELPERS_H

#include <QVector>
#include <vector>
#include <memory>

namespace Models {
    class ArtworkMetadata;
    class ArtworkMetadataLocker;
    class ImageArtwork;
    class VideoArtwork;
}

namespace Helpers {
    void splitImagesVideo(const QVector<Models::ArtworkMetadata *> &artworks, QVector<Models::ArtworkMetadata *> &imageArtworks, QVector<Models::ArtworkMetadata *> &videoArtworks);
    void splitImagesVideo(const std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &rawSnapshot,
                          std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &imagesRawSnapshot,
                          std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &videoRawSnapshot);
}

#endif // ARTWORKSHELPERS_H
