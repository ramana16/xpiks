/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        void addToLibrary(const QVector<Models::ArtworkMetadata *> artworksList);
        void swap(QHash<QString, LocalArtworkData> &hash);
        void saveToFile();
        void searchArtworks(const QStringList &query,
                            std::vector<std::shared_ptr<SuggestionArtwork> > &searchResults,
                            size_t maxResults);
        void cleanupTrash();

    private:
        void doAddToLibrary(const QVector<Models::ArtworkMetadata *> artworksList);

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
