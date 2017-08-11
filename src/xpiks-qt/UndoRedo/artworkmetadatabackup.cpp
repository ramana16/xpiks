/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworkmetadatabackup.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "../Common/defines.h"

UndoRedo::ArtworkMetadataBackup::ArtworkMetadataBackup(Models::ArtworkMetadata *metadata) {
    m_Description = metadata->getDescription();
    m_Title = metadata->getTitle();
    m_KeywordsList = metadata->getKeywords();
    m_IsModified = metadata->isModified();

    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
    if (image != NULL && image->hasVectorAttached()) {
        m_AttachedVector = image->getAttachedVectorPath();
    }
}

UndoRedo::ArtworkMetadataBackup::ArtworkMetadataBackup(const UndoRedo::ArtworkMetadataBackup &copy):
    m_Description(copy.m_Description),
    m_Title(copy.m_Title),
    m_AttachedVector(copy.m_AttachedVector),
    m_KeywordsList(copy.m_KeywordsList),
    m_IsModified(copy.m_IsModified)
{
}

void UndoRedo::ArtworkMetadataBackup::restore(Models::ArtworkMetadata *metadata) const {
    metadata->setDescription(m_Description);
    metadata->setTitle(m_Title);
    metadata->setKeywords(m_KeywordsList);
    if (m_IsModified) { metadata->setModified(); }
    else { metadata->resetModified(); }

    if (!m_AttachedVector.isEmpty()) {
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
        if (image != NULL) {
            image->attachVector(m_AttachedVector);
        } else {
            LOG_WARNING << "Inconsistency for attached vector";
        }
    }
}
