/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAIOSERVICE_H
#define METADATAIOSERVICE_H

#include <QObject>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Suggestion/locallibraryquery.h"
#include "artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
}
namespace MetadataIO {
    class MetadataIOWorker;

    class MetadataIOService: public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        MetadataIOService(QObject *parent = nullptr);

    public:
        void startService();
        void stopService();

    public:
        void cancelBatch(quint32 batchID) const;

    public:
        void readArtwork(Models::ArtworkMetadata *metadata) const;
        void writeArtwork(Models::ArtworkMetadata *metadata) const;
        quint32 readArtworks(const ArtworksSnapshot &snapshot) const;
        void writeArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const;
        void addArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const;

    public:
        void searchArtworks(Suggestion::LocalLibraryQuery *query);

    private slots:
        void workerFinished();

    private:
        MetadataIOWorker *m_MetadataIOWorker;
    };
}

#endif // METADATAIOSERVICE_H
