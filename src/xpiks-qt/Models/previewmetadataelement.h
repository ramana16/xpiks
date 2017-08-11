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

#include "metadataelement.h"

namespace Models {
    class PreviewMetadataElement:
        public MetadataElement
    {
    public:
        PreviewMetadataElement(ArtworkMetadata *metadata, int index):
            MetadataElement(metadata, index),
            m_Flags(0)
        {
            setSelected(true);
        }

        PreviewMetadataElement(PreviewMetadataElement &&other):
            MetadataElement(std::move(other)),
            m_Flags(other.m_Flags)
        {}

        PreviewMetadataElement &operator=(PreviewMetadataElement &&other) {
            m_Flags = other.m_Flags;

            return static_cast<PreviewMetadataElement &>(MetadataElement::operator=(std::move(other)));
        }

        virtual ~PreviewMetadataElement() {}

    private:
        enum PreviewFlags {
            FlagHasTitleMatch = 1 << 0,
            FlagHasDescriptionMatch = 1 << 1,
            FlagHasKeywordsMatch = 1 << 2
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

    private:
        Common::flag_t m_Flags;
    };
}

#endif // PREVIEWMETADATAELEMENT_H
