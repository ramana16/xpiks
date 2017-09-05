/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAREADINGHUB_H
#define METADATAREADINGHUB_H

#include <QAtomicInt>
#include "../Common/readerwriterqueue.h"
#include "artworkssnapshot.h"
#include "originalmetadata.h"
#include "../Helpers/asynccoordinator.h"
#include <Common/baseentity.h>

namespace MetadataIO {
    class MetadataReadingHub: public Common::BaseEntity
    {
    public:
        MetadataReadingHub();

    public:
        void initializeImport(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID);
        void finalizeImport();

    public:
        Helpers::AsyncCoordinator *getCoordinator() { return m_AsyncCoordinator; }

    public:
        void proceedImport(bool ignoreBackups);
        void cancelImport();

    public:
        void push(std::shared_ptr<OriginalMetadata> &item);

    private slots:
        void onCanInitialize(int status);

    private:
        void initializeArtworks();
        void initEmpty();

    private:
        ArtworksSnapshot m_ArtworksToRead;
        Helpers::AsyncCoordinator m_AsyncCoordinator;
        Common::ReaderWriterQueue<OriginalMetadata> m_ImportQueue;
        quint32 m_StorageReadBatchID;
        volatile bool m_IgnoreBackupsAtImport;
        volatile bool m_InitAsEmpty;
    };
}

#endif // METADATAREADINGHUB_H
