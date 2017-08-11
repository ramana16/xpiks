/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../Common/defines.h"
#include "../Suggestion/locallibrary.h"
#include "addtolibraryjobitem.h"

namespace Maintenance {
    AddToLibraryJobItem::AddToLibraryJobItem(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot, Suggestion::LocalLibrary *localLibrary):
        m_ArtworksSnapshot(std::move(artworksSnapshot)),
        m_LocalLibrary(localLibrary)
    {
        Q_ASSERT(localLibrary != NULL);
        Q_ASSERT(m_ArtworksSnapshot != NULL);
        Q_ASSERT(!m_ArtworksSnapshot->getSnapshot().empty());
    }

    void AddToLibraryJobItem::processJob() {
        LOG_DEBUG << "#";
        doAddToLibrary();
    }

    void AddToLibraryJobItem::doAddToLibrary() {
        m_LocalLibrary->doAddToLibrary(m_ArtworksSnapshot);
        m_LocalLibrary->saveToFile();
    }
}
