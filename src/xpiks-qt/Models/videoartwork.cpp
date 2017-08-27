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

#include "videoartwork.h"
#include <QFileInfo>
#include "../Common/defines.h"

namespace Models {
    VideoArtwork::VideoArtwork(const QString &filepath, qint64 ID, qint64 directoryID):
        ArtworkMetadata(filepath, ID, directoryID),
        m_VideoFlags(0)
    {
        m_ThumbnailPath = ":/Graphics/video-icon.svg";
    }

    void VideoArtwork::setThumbnailPath(const QString &filepath) {
        LOG_INFO << filepath;

        Q_ASSERT(!filepath.isEmpty());

        if (QFileInfo(filepath).exists()) {
            m_ThumbnailPath = filepath;
            setThumbnailGeneratedFlag(true);
            emit thumbnailUpdated();
        } else {
            LOG_WARNING << "Wrong thumbnail filepath";
        }
    }

    void VideoArtwork::initializeThumbnailPath(const QString &filepath) {
        if (getThumbnailGeneratedFlag()) { return; }
        if (filepath.isEmpty()) { return; }

        if (QFileInfo(filepath).exists()) {
            m_ThumbnailPath = filepath;
        } else {
            LOG_WARNING << "Wrong thumbnail filepath";
        }
    }
}
