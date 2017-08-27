/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
