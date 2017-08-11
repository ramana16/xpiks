/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOCALLIBRARY_H
#define LOCALLIBRARY_H

#include <memory>
#include <vector>
#include <QHash>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QMutex>
#include <QDataStream>
#include "../Common/baseentity.h"
#include "../Maintenance/locallibraryloadsaveitem.h"
#include "../Maintenance/addtolibraryjobitem.h"
#include "../MetadataIO/artworkmetadatasnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Suggestion {
    class SuggestionArtwork;

    enum LocalArtworkType {
        LocalArtworkImage,
        LocalArtworkVector,
        LocalArtworkOtherArtwork,
        LocalArtworkVideo
    };

    struct LocalArtworkData {
        int m_ArtworkType;
        QString m_Title;
        QString m_Description;
        QStringList m_Keywords;
        QDateTime m_CreationTime;
        QString m_ReservedString;
        int m_ReservedInt;
    };

    QDataStream &operator<<(QDataStream &out, const LocalArtworkData &v);
    QDataStream &operator>>(QDataStream &in, LocalArtworkData &v);

    class LocalLibrary : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        LocalLibrary();
        virtual ~LocalLibrary();

    public:
        const QString &getLibraryPath() const { return m_Filename; }
        void setLibraryPath(const QString &filename) { m_Filename = filename; }
        void addToLibrary(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot);
        void swap(QHash<QString, LocalArtworkData> &hash);
        void saveToFile();
        void searchArtworks(const QStringList &query,
                            std::vector<std::shared_ptr<SuggestionArtwork> > &searchResults,
                            size_t maxResults);
        void cleanupTrash();

    private:
        void doAddToLibrary(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot);

    private:
        friend class Maintenance::AddToLibraryJobItem;
        friend class Maintenance::LocalLibraryLoadSaveItem;

    private:
        QHash<QString, LocalArtworkData> m_LocalArtworks;
        QMutex m_Mutex;
        QString m_Filename;
    };
}

Q_DECLARE_METATYPE(Suggestion::LocalArtworkData)

#endif // LOCALLIBRARY_H
