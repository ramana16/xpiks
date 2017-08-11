/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
            LOG_DEBUG << "Creating snapshot of" << artworksList.size() << "artwork(s)";

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
            LOG_DEBUG << "Creating snapshot of" << artworks.size() << "artwork(s)";
            m_ArtworksSnapshot.reserve(artworks.size());
            for (auto &item: artworks) {
                m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(item));
            }
        }

        ArtworksSnapshot(const std::deque<Models::ArtworkMetadata *> &artworks) {
            LOG_DEBUG << "Creating snapshot of" << artworks.size() << "artwork(s)";
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
