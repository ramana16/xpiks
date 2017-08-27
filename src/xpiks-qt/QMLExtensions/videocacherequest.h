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

#ifndef VIDEOCACHEREQUEST_H
#define VIDEOCACHEREQUEST_H

#include <QString>
#include "../Models/videoartwork.h"
#include "../Common/flags.h"

namespace QMLExtensions {
    class VideoCacheRequest {
    private:
        enum RequestFlags {
            QuickThumbnailFlag = 1 << 0,
            GoodQualityAllowed = 1 << 1,
            RecacheFlag = 1 << 2,
            WithDelayFlag = 1 << 3
        };

    public:
        VideoCacheRequest():
            m_VideoArtwork(NULL),
            m_Flags(0)
        {
            Common::ApplyFlag(m_Flags, true, QuickThumbnailFlag);
            Common::ApplyFlag(m_Flags, true, RecacheFlag);
            // a constructor for separator item
        }

        VideoCacheRequest(Models::VideoArtwork *videoArtwork, bool recache, bool quickThumbnail=true, bool withDelay=false, bool allowGoodQuality=false):
            m_VideoArtwork(videoArtwork),
            m_Flags(0)
        {
            m_VideoArtwork->acquire();

            Common::ApplyFlag(m_Flags, recache, RecacheFlag);
            Common::ApplyFlag(m_Flags, quickThumbnail, QuickThumbnailFlag);
            Common::ApplyFlag(m_Flags, withDelay, WithDelayFlag);
            Common::ApplyFlag(m_Flags, allowGoodQuality, GoodQualityAllowed);
        }

        virtual ~VideoCacheRequest() {
            if (m_VideoArtwork != NULL) {
                m_VideoArtwork->release();
            }
        }

    public:
        const QString &getFilepath() const { return m_VideoArtwork->getFilepath(); }
        bool getNeedRecache() const { return Common::HasFlag(m_Flags, RecacheFlag); }
        bool getWithDelay() const { return Common::HasFlag(m_Flags, WithDelayFlag); }
        bool getIsQuickThumbnail() const { return Common::HasFlag(m_Flags, QuickThumbnailFlag); }
        bool getGoodQualityAllowed() const { return Common::HasFlag(m_Flags, GoodQualityAllowed); }
        Common::ID_t getArtworkID() const { return m_VideoArtwork->getItemID(); }
        size_t getLastKnownIndex() const { return m_VideoArtwork->getLastKnownIndex(); }
        bool isSeparator() const { return (m_VideoArtwork == NULL); }

    public:
        void setGoodQualityRequest() { Common::ApplyFlag(m_Flags, false, QuickThumbnailFlag); }
        void setThumbnailPath(const QString &path) { m_VideoArtwork->setThumbnailPath(path); }

    private:
        Models::VideoArtwork *m_VideoArtwork;
        Common::flag_t m_Flags;
    };
}

#endif // VIDEOCACHEREQUEST_H
