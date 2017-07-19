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

#include <deque>
#include "../Models/artworkmetadata.h"

namespace MetadataIO {
    class ArtworkMetadataSnapshot : public Models::ArtworkMetadataLocker
    {
    public:
        ArtworkMetadataSnapshot(Models::ArtworkMetadata *metadata):
            Models::ArtworkMetadataLocker(metadata)
        {
        }
    };

    template <typename T>
    class ArtworksSnapshotBase {
    public:
        ArtworksSnapshotBase(T &artworksList) {
            LOG_DEBUG << "#";

            m_Snapshot.reserve(artworksList.size());
            for (auto artwork: artworksList) {
                m_Snapshot.emplace_back(new MetadataIO::ArtworkMetadataSnapshot(artwork));
            }
        }

        virtual ~ArtworksSnapshotBase() {
            LOG_DEBUG << "#";
        }

    public:
        std::vector<std::shared_ptr<MetadataIO::ArtworkMetadataSnapshot> > &getSnapshot() { return m_Snapshot; }

    private:
        std::vector<std::shared_ptr<MetadataIO::ArtworkMetadataSnapshot> > m_Snapshot;
    };

    typedef ArtworksSnapshotBase<std::deque<Models::ArtworkMetadata *>> SessionSnapshot;
    typedef ArtworksSnapshotBase<std::vector<Models::ArtworkMetadata *>> ArtworksSnapshot;
    typedef ArtworksSnapshotBase<const QVector<Models::ArtworkMetadata *>> LibrarySnapshot;
}

#endif // ARTWORKMETADATASNAPSHOT_H
