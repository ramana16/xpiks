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

#ifndef ARTWORKMETADATASNAPSHOT_H
#define ARTWORKMETADATASNAPSHOT_H

#include <QString>
#include <deque>
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"

namespace MetadataIO {
    class ArtworkSessionSnapshot
    {
    public:
        ArtworkSessionSnapshot(Models::ArtworkMetadata *metadata) {
            Q_ASSERT(metadata != nullptr);
            m_ArtworkPath = metadata->getFilepath();

            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
            if (image != nullptr && image->hasVectorAttached()){
                m_VectorPath = image->getAttachedVectorPath();
            }
        }

    public:
        const QString &getArtworkFilePath() const { return m_ArtworkPath; }
        const QString &getAttachedVectorPath() const { return m_VectorPath; }

    private:
        QString m_ArtworkPath;
        QString m_VectorPath;
    };

    class SessionSnapshot {
    public:
        SessionSnapshot(const std::deque<Models::ArtworkMetadata *> &artworksList) {
            LOG_DEBUG << "#";

            m_Snapshot.reserve(artworksList.size());
            for (const auto &artwork: artworksList) {
                m_Snapshot.emplace_back(new MetadataIO::ArtworkSessionSnapshot(artwork));
            }
        }

    public:
        std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > &getSnapshot() { return m_Snapshot; }

    private:
        std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > m_Snapshot;
    };

    class ArtworksSnapshot {
    public:
        ArtworksSnapshot(const QVector<Models::ArtworkMetadata *> &artworks) {
            m_ArtworksSnapshot.reserve(artworks.size());
            for (auto &item: artworks) {
                m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(item));
            }
        }

        ArtworksSnapshot(const std::deque<Models::ArtworkMetadata *> &artworks) {
            m_ArtworksSnapshot.reserve(artworks.size());
            for (auto &item: artworks) {
                m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(item));
            }
        }

    public:
        const std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > &getSnapshot() const { return m_ArtworksSnapshot; }

    private:
        std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > m_ArtworksSnapshot;
    };
}

#endif // ARTWORKMETADATASNAPSHOT_H
