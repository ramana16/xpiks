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

#ifndef ARTWORKUPDATEREQUEST_H
#define ARTWORKUPDATEREQUEST_H

#include <QSet>

namespace QMLExtensions {
    class ArtworkUpdateRequest {
    public:
        ArtworkUpdateRequest(qint64 artworkID, size_t lastKnownIndex, const QSet<int> &rolesToUpdate):
            m_RolesToUpdate(rolesToUpdate),
            m_ArtworkID(artworkID),
            m_LastKnownIndex(lastKnownIndex),
            m_GenerationIndex(0),
            m_IsCacheMiss(false)
        {}

    public:
        const QSet<int> &getRolesToUpdate() const { return m_RolesToUpdate; }
        qint64 getArtworkID() const { return m_ArtworkID; }
        size_t getLastKnownIndex() const { return m_LastKnownIndex; }
        int isFirstGeneration() const { return m_GenerationIndex == 0; }
        bool isCacheMiss() const { return m_IsCacheMiss; }

    public:
        void incrementGeneration() { m_GenerationIndex++; }
        void setCacheMiss() { m_IsCacheMiss = true; }

    private:
        QSet<int> m_RolesToUpdate;
        qint64 m_ArtworkID;
        size_t m_LastKnownIndex;
        int m_GenerationIndex;
        bool m_IsCacheMiss;
    };
}

#endif // ARTWORKUPDATEREQUEST_H
