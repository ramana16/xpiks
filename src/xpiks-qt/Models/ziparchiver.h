/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ZIPARCHIVER_H
#define ZIPARCHIVER_H

#include <QFutureWatcher>
#include <QPair>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../MetadataIO/artworkssnapshot.h"

class QStringList;
class QString;

namespace Models {
    class ZipArchiver:
            public QObject,
            public Common::BaseEntity,
            public Helpers::IFileNotAvailableModel
    {
        Q_PROPERTY(int percent READ getPercent NOTIFY percentChanged)
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getHasErrors WRITE setHasErrors NOTIFY hasErrorsChanged)
        Q_PROPERTY(int itemsCount READ getItemsCount NOTIFY itemsCountChanged)
        Q_OBJECT
    public:
        ZipArchiver();
        virtual ~ZipArchiver() { delete m_ArchiveCreator; }

    public:
        int getPercent() const;
        bool getInProgress() const { return m_IsInProgress; }
        bool getHasErrors() const { return m_HasErrors; }
        int getItemsCount() const { return (int)m_ArtworksSnapshot.size(); }

    public:
        void setInProgress(bool value);
        void setHasErrors(bool value);

    signals:
        void inProgressChanged();
        void hasErrorsChanged();
        void percentChanged();
        void itemsCountChanged();
        void startedProcessing();
        void finishedProcessing();
        void requestCloseWindow();

    public slots:
        void archiveCreated(int);
        void allFinished();

    public:
        Q_INVOKABLE void archiveArtworks();
        Q_INVOKABLE void resetModel();

    public:
        void setArtworks(MetadataIO::ArtworksSnapshot &snapshot);
        void resetArtworks();

    protected:
        virtual bool removeUnavailableItems() override;

    private:
        void fillFilenamesHash(QHash<QString, QStringList> &hash);

#ifdef CORE_TESTS
    public:
#else
    protected:
#endif
        const MetadataIO::ArtworksSnapshot &getArtworksSnapshot() const { return m_ArtworksSnapshot; }

    private:
        MetadataIO::ArtworksSnapshot m_ArtworksSnapshot;
        QFutureWatcher<QStringList> *m_ArchiveCreator;
        QAtomicInt m_ProcessedArtworksCount;
        volatile bool m_IsInProgress;
        volatile bool m_HasErrors;
    };
}

#endif // ZIPARCHIVER_H
