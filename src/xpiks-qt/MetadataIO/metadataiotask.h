/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAIOTASK_H
#define METADATAIOTASK_H

#include "../Models/artworkmetadata.h"
#include "../Suggestion/locallibraryquery.h"
#include "../QMLExtensions/artworksupdatehub.h"

namespace MetadataIO {
    class MetadataIOTaskBase: public Models::ArtworkMetadataLocker
    {
    public:
        MetadataIOTaskBase(Models::ArtworkMetadata *metadata):
            Models::ArtworkMetadataLocker(metadata)
        {
        }
    };

    class UpdateHubHighFrequencyModeTask: public MetadataIOTaskBase {
    public:
        UpdateHubHighFrequencyModeTask(QMLExtensions::ArtworksUpdateHub *updateHub):
            MetadataIOTaskBase(nullptr),
            m_UpdateHub(updateHub)
        {
            Q_ASSERT(updateHub != nullptr);
            updateHub->setHighFrequencyMode();
        }

        virtual ~UpdateHubHighFrequencyModeTask() {
            Q_ASSERT(m_UpdateHub != nullptr);
            m_UpdateHub->resetHighFrequencyMode();
        }

    private:
        QMLExtensions::ArtworksUpdateHub *m_UpdateHub;
    };

    class MetadataSearchTask: public MetadataIOTaskBase {
    public:
        MetadataSearchTask(Suggestion::LocalLibraryQuery *query):
            MetadataIOTaskBase(nullptr),
            m_Query(query)
        {
        }

    public:
        Suggestion::LocalLibraryQuery *getQuery() const { return m_Query; }

    private:
        Suggestion::LocalLibraryQuery *m_Query;
    };

    class MetadataCacheSyncTask: public MetadataIOTaskBase {
    public:
        MetadataCacheSyncTask():
            MetadataIOTaskBase(nullptr)
        {
        }
    };

    class MetadataReadWriteTask: public MetadataIOTaskBase {
    public:
        enum ReadWriteAction {
            Read,
            Write,
            Add
        };

    public:
        MetadataReadWriteTask(Models::ArtworkMetadata *metadata, ReadWriteAction readWriteAction):
            MetadataIOTaskBase(metadata),
            m_ReadWriteAction(readWriteAction)
        {}

    public:
        ReadWriteAction getReadWriteAction() const { return m_ReadWriteAction; }

    private:
        ReadWriteAction m_ReadWriteAction;
    };
}

#endif // METADATAIOTASK_H
