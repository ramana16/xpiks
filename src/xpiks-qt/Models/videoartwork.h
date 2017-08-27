/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef VIDEOARTWORK_H
#define VIDEOARTWORK_H

#include "artworkmetadata.h"
#include <QString>
#include "../Common/flags.h"

namespace Models {
    class VideoArtwork: public ArtworkMetadata
    {
        Q_OBJECT
    public:
        VideoArtwork(const QString &filepath, qint64 ID, qint64 directoryID);

    private:
        enum VideoArtworkFlags {
            FlagThumbnailGenerated = 1 << 0
        };

        inline bool getThumbnailGeneratedFlag() const { return Common::HasFlag(m_VideoFlags, FlagThumbnailGenerated); }
        inline void setThumbnailGeneratedFlag(bool value) { Common::ApplyFlag(m_VideoFlags, value, FlagThumbnailGenerated); }

    public:
        bool isThumbnailGenerated() const { return getThumbnailGeneratedFlag(); }
        virtual const QString &getThumbnailPath() const override { return m_ThumbnailPath; }
        void setThumbnailPath(const QString &filepath);
        void initializeThumbnailPath(const QString &filepath);

    private:
        QString m_ThumbnailPath;
        volatile Common::flag_t m_VideoFlags;
    };
}

#endif // VIDEOARTWORK_H
