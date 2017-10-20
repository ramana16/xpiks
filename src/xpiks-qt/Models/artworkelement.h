/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAELEMENT_H
#define METADATAELEMENT_H

#include <QString>
#include <QSet>
#include "artworkmetadata.h"
#include "../Common/flags.h"

namespace Models {
    class ArtworkElement: public ArtworkMetadataLocker
    {
    public:
        ArtworkElement(ArtworkMetadata *artwork):
            ArtworkMetadataLocker(artwork),
            m_Flags(0)
        {
            Q_ASSERT(artwork != nullptr);
        }

    private:
        enum ArtworkElementFlags {
            IsSelectedFlag = 1 << 0
        };

        bool getIsSelectedFlag() const { return Common::HasFlag(m_Flags, IsSelectedFlag); }
        void setIsSelectedFlag(bool value) { Common::ApplyFlag(m_Flags, value, IsSelectedFlag); }

    public:
        bool getIsSelected() const { return getIsSelectedFlag(); }
        void setIsSelected(bool value) { setIsSelectedFlag(value); }

    public:
        int getOriginalIndex() const { return getArtworkMetadata()->getLastKnownIndex(); }

#ifdef CORE_TESTS
        void freeMetadata() {
            // BUMP
        }
#endif

    private:
        ArtworkElement(const ArtworkElement&);
        ArtworkElement &operator=(const ArtworkElement &);

    protected:
        Common::flag_t m_Flags;
    };
}

#endif // METADATAELEMENT_H
