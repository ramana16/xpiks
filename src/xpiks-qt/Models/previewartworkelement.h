/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PREVIEWMETADATAELEMENT_H
#define PREVIEWMETADATAELEMENT_H

#include "artworkelement.h"

namespace Models {
    class PreviewArtworkElement: public ArtworkElement
    {
    public:
        PreviewArtworkElement(ArtworkMetadata *metadata):
            ArtworkElement(metadata)
        {
            setIsSelected(true);
        }

    private:
        enum PreviewFlags {
            FlagHasTitleMatch = 1 << 1,
            FlagHasDescriptionMatch = 1 << 2,
            FlagHasKeywordsMatch = 1 << 3
        };

        inline bool getHasTitleMatchFlag() const { return Common::HasFlag(m_Flags, FlagHasTitleMatch); }
        inline bool getHasDescriptionMatchFlag() const { return Common::HasFlag(m_Flags, FlagHasDescriptionMatch); }
        inline bool getHasKeywordsMatchFlag() const { return Common::HasFlag(m_Flags, FlagHasKeywordsMatch); }

        inline void setHasTitleMatchFlag(bool value) { Common::ApplyFlag(m_Flags, value, FlagHasTitleMatch); }
        inline void setHasDescriptionMatchFlag(bool value) { Common::ApplyFlag(m_Flags, value, FlagHasDescriptionMatch); }
        inline void setHasKeywordsMatchFlag(bool value) { Common::ApplyFlag(m_Flags, value, FlagHasKeywordsMatch); }

    public:
        bool hasTitleMatch() const { return getHasTitleMatchFlag(); }
        bool hasDescriptionMatch() const { return getHasDescriptionMatchFlag(); }
        bool hasKeywordsMatch() const { return getHasKeywordsMatchFlag(); }

    public:
        void setHasTitleMatch(bool value) { setHasTitleMatchFlag(value); }
        void setHasDescriptionMatch(bool value) { setHasDescriptionMatchFlag(value); }
        void setHasKeywordsMatch(bool value) { setHasKeywordsMatchFlag(value); }
    };
}

#endif // PREVIEWMETADATAELEMENT_H
