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
        ArtworkSessionSnapshot(Models::ArtworkMetadata *metadata);

    public:
        const QString &getArtworkFilePath() const { return m_ArtworkPath; }
        const QString &getAttachedVectorPath() const { return m_VectorPath; }

    private:
        QString m_ArtworkPath;
        QString m_VectorPath;
    };

    class SessionSnapshot {
    public:
        SessionSnapshot(const std::deque<Models::ArtworkMetadata *> &artworksList, const QStringList &fullDirectories);

        SessionSnapshot(SessionSnapshot &&other) {
            m_ArtworksSnapshot.swap(other.m_ArtworksSnapshot);
            m_DirectoriesSnapshot.swap(other.m_DirectoriesSnapshot);
        }

        SessionSnapshot &operator=(SessionSnapshot &&other) {
            if (this != &other) {
                m_ArtworksSnapshot.swap(other.m_ArtworksSnapshot);
                m_DirectoriesSnapshot.swap(other.m_DirectoriesSnapshot);
            }

            return *this;
        }

    public:
        std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > &getSnapshot() { return m_ArtworksSnapshot; }
        const QStringList &getDirectoriesSnapshot() { return m_DirectoriesSnapshot; }

    private:
        std::vector<std::shared_ptr<MetadataIO::ArtworkSessionSnapshot> > m_ArtworksSnapshot;
        QStringList m_DirectoriesSnapshot;
    };

    // designed to be used only temporarily
    // artworks are not locked and therefore can be deleted
    typedef std::vector<Models::ArtworkMetadata*> WeakArtworksSnapshot;

    class ArtworksSnapshot {
    public:
        typedef std::vector<std::shared_ptr<Models::ArtworkMetadataLocker> > Container;

    public:
        ArtworksSnapshot() { }
        ArtworksSnapshot(const WeakArtworksSnapshot &artworks);
        ArtworksSnapshot(const std::deque<Models::ArtworkMetadata *> &artworks);
        ArtworksSnapshot(Container &rawSnapshot);
        ArtworksSnapshot(ArtworksSnapshot &&other);
        ArtworksSnapshot &operator=(ArtworksSnapshot &&other);
        ArtworksSnapshot &operator=(const ArtworksSnapshot &other);
        ArtworksSnapshot(const ArtworksSnapshot &other);
        virtual ~ArtworksSnapshot();

    public:
        const Container &getRawData() const { return m_ArtworksSnapshot; }
        const WeakArtworksSnapshot &getWeakSnapshot() const { return m_RawArtworks; }
        size_t size() const { return m_ArtworksSnapshot.size(); }

    public:
        void reserve(size_t size) { m_ArtworksSnapshot.reserve(size); m_RawArtworks.reserve((int)size); }
        void append(Models::ArtworkMetadata *artwork) {
            m_ArtworksSnapshot.emplace_back(new Models::ArtworkMetadataLocker(artwork));
            m_RawArtworks.push_back(artwork);
        }
        void append(const WeakArtworksSnapshot &artworks);
        void append(const std::deque<Models::ArtworkMetadata *> &artworks);
        void set(Container &rawSnapshot);
        void copy(const ArtworksSnapshot &other);
        void remove(size_t index);
        Models::ArtworkMetadata *get(size_t i) const { Q_ASSERT(i < m_ArtworksSnapshot.size()); return m_ArtworksSnapshot.at(i)->getArtworkMetadata(); }
        const std::shared_ptr<Models::ArtworkMetadataLocker> &at(size_t i) const { Q_ASSERT(i < m_ArtworksSnapshot.size()); return m_ArtworksSnapshot.at(i); }
        void clear();
        bool empty() const { Q_ASSERT(m_ArtworksSnapshot.size() == m_RawArtworks.size()); return m_ArtworksSnapshot.empty(); }

    private:
        Container m_ArtworksSnapshot;
        // TODO: deprecate and remove raw items
        WeakArtworksSnapshot m_RawArtworks;
    };
}

#endif // ARTWORKMETADATASNAPSHOT_H
