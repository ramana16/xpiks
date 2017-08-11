/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BACKUPSAVERSERVICE_H
#define BACKUPSAVERSERVICE_H

#include <QObject>
#include <QVector>

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class BackupSaverWorker;

    class BackupSaverService : public QObject
    {
        Q_OBJECT
    public:
        BackupSaverService();

    public:
        void startSaving();
        void stopSaving();
        void saveArtwork(Models::ArtworkMetadata *metadata) const;
        void saveArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const;
        void readArtwork(Models::ArtworkMetadata *metadata) const;
        void readArtworks(const QVector<Models::ArtworkMetadata *> &artworks) const;

    signals:
        void cancelSaving();

    private slots:
        void workerFinished();

    private:
        BackupSaverWorker *m_BackupWorker;
    };
}

#endif // BACKUPSAVERSERVICE_H
