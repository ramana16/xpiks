/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QtAlgorithms>
#include <QFile>
#include <QDataStream>
#include "locallibrary.h"
#include "../Models/artworkmetadata.h"
#include "suggestionartwork.h"
#include "../Common/defines.h"
#include "../Common/basickeywordsmodel.h"
#include "../Models/imageartwork.h"
#include "../Maintenance/maintenanceservice.h"
#include "../Maintenance/addtolibraryjobitem.h"
#include "../Commands/commandmanager.h"

namespace Suggestion {
    QDataStream &operator<<(QDataStream &out, const LocalArtworkData &v) {
        out << v.m_ArtworkType << v.m_Title << v.m_Description << v.m_Keywords << v.m_CreationTime << v.m_ReservedString << v.m_ReservedInt;
        return out;
    }

    QDataStream &operator>>(QDataStream &in, LocalArtworkData &v) {
        in >> v.m_ArtworkType >> v.m_Title >> v.m_Description >> v.m_Keywords >> v.m_CreationTime >> v.m_ReservedString >> v.m_ReservedInt;
        return in;
    }

    LocalLibrary::LocalLibrary():
        QObject(),
        Common::BaseEntity()
    {
    }

    LocalLibrary::~LocalLibrary() {
    }

    void LocalLibrary::addToLibrary(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot) {
        LOG_DEBUG << "#";
        // adding to library will be complicated in future
        // so always do it in the background
#ifndef INTEGRATION_TESTS
        Maintenance::MaintenanceService *maintenanceService = m_CommandManager->getMaintenanceService();
        maintenanceService->addArtworksToLibrary(artworksSnapshot, this);
#else
        doAddToLibrary(artworksSnapshot);
#endif
    }

    void LocalLibrary::swap(QHash<QString, LocalArtworkData> &hash) {
        QMutexLocker locker(&m_Mutex);
        m_LocalArtworks.swap(hash);
        LOG_DEBUG << "swapped with read from db.";
    }

    void LocalLibrary::saveToFile() {
        QFile file(m_Filename);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data

            m_Mutex.lock();
            {
                out << m_LocalArtworks;
            }
            m_Mutex.unlock();

            file.close();

            LOG_INFO << "saved to" << m_Filename;
        }
    }

    void LocalLibrary::searchArtworks(const QStringList &query,
                                      std::vector<std::shared_ptr<SuggestionArtwork> > &searchResults,
                                      size_t maxResults) {
        LOG_INFO << "max results" << maxResults;
        QMutexLocker locker(&m_Mutex);

        QHashIterator<QString, LocalArtworkData> i(m_LocalArtworks);
        QVector<QPair<QDateTime, QString> > results;

        while (i.hasNext()) {
            i.next();

            bool anyError = false;

            auto &localData = i.value();

            const QStringList &keywords = localData.m_Keywords;
            foreach (const QString &searchTerm, query) {
                bool containsTerm = false;

                foreach (const QString &keyword, keywords) {
                    if (keyword.contains(searchTerm, Qt::CaseInsensitive)) {
                        containsTerm = true;
                        break;
                    }
                }

                if (localData.m_Title.contains(searchTerm, Qt::CaseInsensitive)) {
                    containsTerm = true;
                    break;
                }

                if (localData.m_Description.contains(searchTerm, Qt::CaseInsensitive)) {
                    containsTerm = true;
                    break;
                }

                if (!containsTerm) {
                    anyError = true;
                    break;
                }
            }

            if (!anyError) {
                if (QFileInfo(i.key()).exists()) {
                    results.append(qMakePair(localData.m_CreationTime, i.key()));

                    if (searchResults.size() >= maxResults) {
                        break;
                    }
                }
            }
        }

        // earlier datetimes go last
        qSort(results.begin(), results.end(),
              [](const QPair<QDateTime, QString> &a1, const QPair<QDateTime, QString> &a2) -> bool {
            return a1.first > a2.first;
        });

        foreach (auto &pair, results) {
            auto &filepath = pair.second;
            Q_ASSERT(m_LocalArtworks.contains(filepath));
            auto &localData = m_LocalArtworks[filepath];
            searchResults.emplace_back(new SuggestionArtwork(filepath, localData.m_Title, localData.m_Description, localData.m_Keywords));
        }
    }

    void LocalLibrary::cleanupTrash() {
        QMutexLocker locker(&m_Mutex);

        QStringList itemsToRemove;
        QHashIterator<QString, LocalArtworkData> i(m_LocalArtworks);

        while (i.hasNext()) {
            i.next();

            QFile file(i.key());
            if (!file.exists()) {
                itemsToRemove.append(i.key());
            }
        }

        foreach (const QString &item, itemsToRemove) {
            m_LocalArtworks.remove(item);
        }

        LOG_INFO << itemsToRemove.count() << "item(s) removed.";
    }

    void LocalLibrary::doAddToLibrary(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot) {
        auto &artworksList = artworksSnapshot->getSnapshot();

        const size_t length = artworksList.size();

        LOG_INFO << length << "file(s)";

        QMutexLocker locker(&m_Mutex);

        for (size_t i = 0; i < length; ++i) {
            Models::ArtworkMetadata *metadata = artworksList.at(i)->getArtworkMetadata();
            const QString &filepath = metadata->getFilepath();
            QFileInfo fi(filepath);

            LocalArtworkData data;
            data.m_ArtworkType = LocalArtworkImage;
            data.m_Title = metadata->getTitle();
            data.m_Description = metadata->getDescription();
            data.m_Keywords = metadata->getKeywords();
            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
            if (image != nullptr) {
                if (image->hasVectorAttached()) {
                    data.m_ArtworkType = LocalArtworkVector;
                }
            }

            if (fi.exists()) {
                data.m_CreationTime = fi.created();
            }

            // replaces if exists
            m_LocalArtworks.insert(filepath, data);
        }

        LOG_INFO << length << "item(s) updated or added";
    }
}
