/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CACHEDARTWORK_H
#define CACHEDARTWORK_H

#include <QDataStream>
#include <QStringList>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../Common/flags.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    struct CachedArtwork {
        enum CachedArtworkType {
            Unknown = 0,
            Image = 1,
            Vector = 2,
            Video = 3,
            Other
        };

        CachedArtwork();
        CachedArtwork(Models::ArtworkMetadata *metadata);
        CachedArtwork(const CachedArtwork &from);
        CachedArtwork &operator=(const CachedArtwork &other);

        void initSerializationVersion();

        quint8 m_Version;
        // BEGIN of version 1 data
        quint32 m_ArtworkType; // 32-bit for allignment
        Common::flag_t m_Flags;
        quint64 m_FilesizeBytes;
        quint16 m_CategoryID_1;
        quint16 m_CategoryID_2;
        QString m_Filepath;
        QString m_Title;
        QString m_Description;
        QString m_ThumbnailPath;
        /*VIDEO*/QString m_CodecName;
        /*VECTOR*/QString m_AttachedVector;
        /*PHOTO*/QDateTime m_CreationTime;
        QStringList m_Keywords;
        QVector<quint16> m_ModelReleaseIDs;
        QVector<quint16> m_PropertyReleaseIDs;
        // END of version 1 data
    };

    QDataStream &operator<<(QDataStream &out, const CachedArtwork &v);
    QDataStream &operator>>(QDataStream &in, CachedArtwork &v);
}

#endif // CACHEDARTWORK_H
