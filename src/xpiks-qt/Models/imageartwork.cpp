/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "imageartwork.h"
#include "../Common/defines.h"
#include "../MetadataIO/cachedartwork.h"
#include "../MetadataIO/originalmetadata.h"

namespace Models {
    ImageArtwork::ImageArtwork(const QString &filepath, qint64 ID, qint64 directoryID):
        ArtworkMetadata(filepath, ID, directoryID),
        m_ImageFlags(0)
    {
    }

    void ImageArtwork::attachVector(const QString &vectorFilepath) {
        LOG_INFO << "Attaching vector file:" << vectorFilepath << "to file" << getFilepath();
        setHasVectorAttachedFlag(true);
        m_AttachedVector = vectorFilepath;
    }

    void ImageArtwork::detachVector() {
        LOG_INFO << "#";
        setHasVectorAttachedFlag(false);
        m_AttachedVector.clear();
    }

    bool ImageArtwork::initFromOriginUnsafe(const MetadataIO::OriginalMetadata &originalMetadata) {
        setImageSize(originalMetadata.m_ImageSize);
        setDateTimeOriginal(originalMetadata.m_DateTimeOriginal);

        // TODO: check if this is needed
        return false;
    }

    bool ImageArtwork::initFromStorageUnsafe(const MetadataIO::CachedArtwork &cachedArtwork) {
        //setImageSize(cachedArtwork.m_ImageSize);
        setDateTimeOriginal(cachedArtwork.m_CreationTime);

        // TODO: check if this is needed
        return false;
    }
}
