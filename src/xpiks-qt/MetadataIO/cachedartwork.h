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

        quint32 m_ArtworkType; // 32-bit for allignment
        Common::flag_t m_Flags;
        quint64 m_FilesizeBytes;
        quint16 m_CategoryID_1;
        quint16 m_CategoryID_2;
        QString m_Filepath;
        QString m_Title;
        QString m_Description;
        QString m_AttachedVector;
        QString m_ThumbnailPath;
        QStringList m_Keywords;
        QVector<quint16> m_ModelReleases;
        QVector<quint16> m_PropertyReleases;
        QDateTime m_CreationTime;
        // reserved for future demands
        QHash<qint32, QByteArray> m_AdditionalData;

        // temporary data
    };

    QDataStream &operator<<(QDataStream &out, const CachedArtwork &v);
    QDataStream &operator>>(QDataStream &in, CachedArtwork &v);
}

#endif // CACHEDARTWORK_H
