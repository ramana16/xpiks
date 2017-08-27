/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cachedartwork.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "../Models/videoartwork.h"

namespace MetadataIO {

    CachedArtwork::CachedArtwork():
        m_ArtworkType(Unknown),
        m_Flags(0),
        m_FilesizeBytes(0),
        m_CategoryID_1(0),
        m_CategoryID_2(0)
    {
    }

    CachedArtwork::CachedArtwork(Models::ArtworkMetadata *metadata):
        m_Flags(0),
        m_CategoryID_1(0),
        m_CategoryID_2(0)
    {
        m_ArtworkType = Unknown;

        m_FilesizeBytes = metadata->getFileSize();
        m_Filepath = metadata->getFilepath();
        m_Title = metadata->getTitle();
        m_Description = metadata->getDescription();
        m_Keywords = metadata->getKeywords();
        m_ThumbnailPath = metadata->getThumbnailPath();

        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
        if (metadata != nullptr) {
            m_ArtworkType = image->hasVectorAttached() ? Vector : Image;
            m_AttachedVector = image->getAttachedVectorPath();
            m_CreationTime = image->getDateTimeOriginal();
        } else {
            Models::VideoArtwork *video = dynamic_cast<Models::VideoArtwork*>(metadata);
            Q_UNUSED(video);
            m_ArtworkType = Video;
        }
    }

    CachedArtwork::CachedArtwork(const CachedArtwork &from):
        m_ArtworkType(from.m_ArtworkType),
        m_Flags(from.m_Flags),
        m_FilesizeBytes(from.m_FilesizeBytes),
        m_CategoryID_1(from.m_CategoryID_1),
        m_CategoryID_2(from.m_CategoryID_2),
        m_Filepath(from.m_Filepath),
        m_Title(from.m_Title),
        m_Description(from.m_Description),
        m_AttachedVector(from.m_AttachedVector),
        m_ThumbnailPath(from.m_ThumbnailPath),
        m_Keywords(from.m_Keywords),
        m_ModelReleases(from.m_ModelReleases),
        m_PropertyReleases(from.m_PropertyReleases),
        m_CreationTime(from.m_CreationTime),
        m_AdditionalData(from.m_AdditionalData)
    {
    }

    CachedArtwork &CachedArtwork::operator=(const CachedArtwork &other) {
        m_ArtworkType = other.m_ArtworkType;
        m_Flags = other.m_Flags;
        m_FilesizeBytes = other.m_FilesizeBytes;
        m_CategoryID_1 = other.m_CategoryID_1;
        m_CategoryID_2 = other.m_CategoryID_2;
        m_Filepath = other.m_Filepath;
        m_Title = other.m_Title;
        m_Description = other.m_Description;
        m_AttachedVector = other.m_AttachedVector;
        m_ThumbnailPath = other.m_ThumbnailPath;
        m_Keywords = other.m_Keywords;
        m_ModelReleases = other.m_ModelReleases;
        m_PropertyReleases = other.m_PropertyReleases;
        m_CreationTime = other.m_CreationTime;
        m_AdditionalData = other.m_AdditionalData;

        return *this;
    }

    QDataStream &operator<<(QDataStream &out, const CachedArtwork &v) {
        out << v.m_ArtworkType << v.m_Flags << v.m_FilesizeBytes << v.m_CategoryID_1 <<
               v.m_CategoryID_2 << v.m_Filepath << v.m_Title << v.m_Description <<
               v.m_AttachedVector << v.m_ThumbnailPath << v.m_Keywords << v.m_ModelReleases <<
               v.m_PropertyReleases << v.m_CreationTime << v.m_AdditionalData;

        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedArtwork &v) {
        in >> v.m_ArtworkType >> v.m_Flags >> v.m_FilesizeBytes >> v.m_CategoryID_1 >>
                v.m_CategoryID_2 >> v.m_Filepath >> v.m_Title >> v.m_Description >>
                v.m_AttachedVector >> v.m_ThumbnailPath >> v.m_Keywords >> v.m_ModelReleases >>
                v.m_PropertyReleases >> v.m_CreationTime >> v.m_AdditionalData;

        return in;
    }
}
