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

namespace Models {
    // sort of smart pointer over ArtworkMetadata
    class MetadataElement
    {
    public:
        MetadataElement(ArtworkMetadata *metadata, int index):
            m_ArtworkMetadata(metadata),
            m_OriginalIndex(index),
            m_IsSelected(false)
        {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->acquire();
            }
        }

        MetadataElement(const MetadataElement &other):
            m_ArtworkMetadata(other.m_ArtworkMetadata),
            m_OriginalIndex(other.m_OriginalIndex),
            m_IsSelected(other.m_IsSelected)
        {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->acquire();
            }
        }

        MetadataElement(MetadataElement &&other):
            m_ArtworkMetadata(other.m_ArtworkMetadata),
            m_OriginalIndex(other.m_OriginalIndex),
            m_IsSelected(other.m_IsSelected)
        {
            other.m_ArtworkMetadata = nullptr;
        }

        MetadataElement &operator=(MetadataElement &&other) {
            if (this != &other) {
                releaseArtwork();

                m_ArtworkMetadata = other.m_ArtworkMetadata;
                m_OriginalIndex = other.m_OriginalIndex;
                m_IsSelected = other.m_IsSelected;

                other.m_ArtworkMetadata = nullptr;
            }

            return *this;
        }

        virtual ~MetadataElement() {
            releaseArtwork();
        }

    public:        
        bool isSelected() const { return m_IsSelected; }
        void setSelected(bool selected) { m_IsSelected = selected; }

    public:
        ArtworkMetadata *getOrigin() const { return m_ArtworkMetadata; }
        int getOriginalIndex() const { return m_OriginalIndex; }

#ifdef CORE_TESTS
        void freeMetadata() {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->release();
                m_ArtworkMetadata->deleteLater();
                m_ArtworkMetadata = nullptr;
            }
        }
#endif

    private:
        void releaseArtwork() {
            if (m_ArtworkMetadata != nullptr) {
                if (m_ArtworkMetadata->release()) {
                    LOG_DEBUG << "Item could have been removed";
                }
            }
        }

        MetadataElement &operator=(const MetadataElement &);

    private:
        ArtworkMetadata *m_ArtworkMetadata;
        int m_OriginalIndex;
        bool m_IsSelected;
    };
}

#endif // METADATAELEMENT_H
