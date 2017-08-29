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
#include "../Common/version.h"

namespace MetadataIO {

    CachedArtwork::CachedArtwork():
        m_Version(0),
        m_ArtworkType(Unknown),
        m_Flags(0),
        m_FilesizeBytes(0),
        m_CategoryID_1(0),
        m_CategoryID_2(0)
    {
        initSerializationVersion();
    }

    CachedArtwork::CachedArtwork(Models::ArtworkMetadata *metadata):
        m_Version(0),
        m_Flags(0),
        m_CategoryID_1(0),
        m_CategoryID_2(0)
    {
        initSerializationVersion();

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
            m_ArtworkType = Video;
            m_CodecName = video->getCodecName();
        }
    }

    CachedArtwork::CachedArtwork(const CachedArtwork &from):
        m_Version(from.m_Version),
        m_ArtworkType(from.m_ArtworkType),
        m_Flags(from.m_Flags),
        m_FilesizeBytes(from.m_FilesizeBytes),
        m_CategoryID_1(from.m_CategoryID_1),
        m_CategoryID_2(from.m_CategoryID_2),
        m_Filepath(from.m_Filepath),
        m_Title(from.m_Title),
        m_Description(from.m_Description),
        m_ThumbnailPath(from.m_ThumbnailPath),
        m_CodecName(from.m_CodecName),
        m_AttachedVector(from.m_AttachedVector),
        m_CreationTime(from.m_CreationTime),
        m_Keywords(from.m_Keywords),
        m_ModelReleaseIDs(from.m_ModelReleaseIDs),
        m_PropertyReleaseIDs(from.m_PropertyReleaseIDs)
    {
    }

    CachedArtwork &CachedArtwork::operator=(const CachedArtwork &other) {
        m_Version = other.m_Version;
        m_ArtworkType = other.m_ArtworkType;
        m_Flags = other.m_Flags;
        m_FilesizeBytes = other.m_FilesizeBytes;
        m_CategoryID_1 = other.m_CategoryID_1;
        m_CategoryID_2 = other.m_CategoryID_2;
        m_Filepath = other.m_Filepath;
        m_Title = other.m_Title;
        m_Description = other.m_Description;
        m_ThumbnailPath = other.m_ThumbnailPath;
        m_CodecName = other.m_CodecName;
        m_AttachedVector = other.m_AttachedVector;
        m_CreationTime = other.m_CreationTime;
        m_Keywords = other.m_Keywords;
        m_ModelReleaseIDs = other.m_ModelReleaseIDs;
        m_PropertyReleaseIDs = other.m_PropertyReleaseIDs;

        return *this;
    }

    void CachedArtwork::initSerializationVersion() {
        if (XPIKS_MAJOR_VERSION_CHECK(1, 5) ||
                XPIKS_MAJOR_VERSION_CHECK(1, 4)) {
            m_Version = 1;
        }
    }

    QDataStream &operator<<(QDataStream &out, const CachedArtwork &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(!XPIKS_VERSION_CHECK(1, 5, 0));
#ifndef TRAVIS_CI
        out.setVersion(QDataStream::Qt_5_6);
#endif

        out << v.m_Version;
        out << v.m_ArtworkType;
        out << v.m_Flags;
        out << v.m_FilesizeBytes;
        out << v.m_CategoryID_1;
        out << v.m_CategoryID_2;
        out << v.m_Filepath;
        out << v.m_Title;
        out << v.m_Description;
        out << v.m_ThumbnailPath;

        if (v.m_ArtworkType == CachedArtwork::Video) {
            out << v.m_CodecName;
        }

        if (v.m_ArtworkType == CachedArtwork::Vector) {
            out << v.m_AttachedVector;
        }

        if (v.m_ArtworkType == CachedArtwork::Image) {
            out << v.m_CreationTime;
        }

        out << v.m_Keywords;
        out << v.m_ModelReleaseIDs;
        out << v.m_PropertyReleaseIDs;

        Q_ASSERT(out.status() == QDataStream::Ok);

        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedArtwork &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(!XPIKS_VERSION_CHECK(1, 5, 0));
#ifndef TRAVIS_CI
        in.setVersion(QDataStream::Qt_5_6);
#endif

        in >> v.m_Version;
        in >> v.m_ArtworkType;
        in >> v.m_Flags;
        in >> v.m_FilesizeBytes;
        in >> v.m_CategoryID_1;
        in >> v.m_CategoryID_2;
        in >> v.m_Filepath;
        in >> v.m_Title;
        in >> v.m_Description;
        in >> v.m_ThumbnailPath;

        if (v.m_ArtworkType == CachedArtwork::Video) {
            in >> v.m_CodecName;
        }

        if (v.m_ArtworkType == CachedArtwork::Vector) {
            in >> v.m_AttachedVector;
        }

        if (v.m_ArtworkType == CachedArtwork::Image) {
            in >> v.m_CreationTime;
        }

        in >> v.m_Keywords;
        in >> v.m_ModelReleaseIDs;
        in >> v.m_PropertyReleaseIDs;

        Q_ASSERT(in.status() == QDataStream::Ok);

        return in;
    }
}
