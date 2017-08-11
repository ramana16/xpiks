/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ADDTOLIBRARYJOBITEM_H
#define ADDTOLIBRARYJOBITEM_H

#include <QVector>
#include "../MetadataIO/artworkmetadatasnapshot.h"
#include "imaintenanceitem.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Suggestion {
    class LocalLibrary;
}

namespace Maintenance {
    class AddToLibraryJobItem : public IMaintenanceItem
    {
    public:
        AddToLibraryJobItem(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot, Suggestion::LocalLibrary *localLibrary);

    public:
        virtual void processJob() override;

    private:
        void doAddToLibrary();

    private:
        std::unique_ptr<MetadataIO::ArtworksSnapshot> m_ArtworksSnapshot;
        Suggestion::LocalLibrary *m_LocalLibrary;
    };
}

#endif // ADDTOLIBRARYJOBITEM_H
