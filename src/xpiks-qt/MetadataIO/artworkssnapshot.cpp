/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworkssnapshot.h"

namespace MetadataIO {
    ArtworkSessionSnapshot::ArtworkSessionSnapshot(Models::ArtworkMetadata *metadata) {
        Q_ASSERT(metadata != nullptr);
        m_ArtworkPath = metadata->getFilepath();

        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
        if (image != nullptr && image->hasVectorAttached()){
            m_VectorPath = image->getAttachedVectorPath();
        }
    }

    SessionSnapshot::SessionSnapshot(const std::deque<Models::ArtworkMetadata *> &artworksList, const QStringList &fullDirectories):
        m_DirectoriesSnapshot(fullDirectories)
    {
        LOG_DEBUG << "Creating snapshot of" << artworksList.size() << "artwork(s)";

        m_ArtworksSnapshot.reserve(artworksList.size());
        for (const auto &artwork: artworksList) {
            m_ArtworksSnapshot.emplace_back(new MetadataIO::ArtworkSessionSnapshot(artwork));
        }
    }

    ArtworksSnapshot::ArtworksSnapshot(const WeakArtworksSnapshot &artworks) {
        append(artworks);
    }

    ArtworksSnapshot::ArtworksSnapshot(const std::deque<Models::ArtworkMetadata *> &artworks) {
       append(artworks);
    }

    ArtworksSnapshot::ArtworksSnapshot(Container &rawSnapshot) {
        set(rawSnapshot);
    }

    ArtworksSnapshot::ArtworksSnapshot(ArtworksSnapshot &&other) {
        m_ArtworksSnapshot.swap(other.m_ArtworksSnapshot);
        m_RawArtworks.swap(other.m_RawArtworks);
    }

    ArtworksSnapshot &ArtworksSnapshot::operator=(ArtworksSnapshot &&other) {
        if (this != &other) {
            LOG_DEBUG << "Moving snapshot of" << other.m_ArtworksSnapshot.size() << "item(s)";
            m_ArtworksSnapshot = std::move(other.m_ArtworksSnapshot);
            m_RawArtworks = std::move(other.m_RawArtworks);
        }

        return *this;
    }

    ArtworksSnapshot &ArtworksSnapshot::operator=(const ArtworksSnapshot &other) {
        if (this != &other) {
            copy(other);
        }

        return *this;
    }

    ArtworksSnapshot::ArtworksSnapshot(const ArtworksSnapshot &other)
    {
        copy(other);
    }

    ArtworksSnapshot::~ArtworksSnapshot() {
        LOG_DEBUG << "Destroying snapshot of" << m_ArtworksSnapshot.size() << "artwork(s)";
    }

    void ArtworksSnapshot::append(const WeakArtworksSnapshot &artworks) {
        m_RawArtworks.insert(m_RawArtworks.end(), artworks.begin(), artworks.end());

        LOG_DEBUG << "Appending snapshot of" << artworks.size() << "artwork(s)";
        m_ArtworksSnapshot.reserve(m_ArtworksSnapshot.size() + artworks.size());
        for (auto &item: artworks) {
            m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(item));
        }
    }

    void ArtworksSnapshot::append(const std::deque<Models::ArtworkMetadata *> &artworks) {
        LOG_DEBUG << "Appending snapshot of" << artworks.size() << "artwork(s)";
        m_ArtworksSnapshot.reserve(m_ArtworksSnapshot.size() + artworks.size());
        m_RawArtworks.reserve(m_RawArtworks.size() + (int)artworks.size());
        for (auto &item: artworks) {
            m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(item));
            m_RawArtworks.push_back(item);
        }
    }

    void ArtworksSnapshot::set(Container &rawSnapshot) {
        clear();
        LOG_DEBUG << "Setting snapshot of" << rawSnapshot.size() << "artwork(s)";

        m_RawArtworks.reserve((int)rawSnapshot.size());
        for (auto &item: rawSnapshot) {
            m_RawArtworks.push_back(item->getArtworkMetadata());
        }

        m_ArtworksSnapshot = std::move(rawSnapshot);
    }

    void ArtworksSnapshot::copy(const ArtworksSnapshot &other) {
        clear();
        LOG_DEBUG << "Copying snapshot of" << other.m_ArtworksSnapshot.size() << "item(s)";

        m_RawArtworks.insert(m_RawArtworks.end(), other.m_RawArtworks.begin(), other.m_RawArtworks.end());
        m_ArtworksSnapshot = other.m_ArtworksSnapshot;
    }

    void ArtworksSnapshot::remove(size_t index) {
        Q_ASSERT(m_ArtworksSnapshot.size() == m_RawArtworks.size());
        if (index >= m_ArtworksSnapshot.size()) { return; }

        m_RawArtworks.erase(m_RawArtworks.begin() + index);
        m_ArtworksSnapshot.erase(m_ArtworksSnapshot.begin() + index);
    }

    void ArtworksSnapshot::clear() {
        LOG_DEBUG << "Removing" << size() << "item(s)";
        m_RawArtworks.clear();
        m_ArtworksSnapshot.clear();
    }
}
