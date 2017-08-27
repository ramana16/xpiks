/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ziparchiver.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "../Helpers/filehelpers.h"
#include "../Common/defines.h"

#ifndef CORE_TESTS
#include "../Helpers/ziphelper.h"
#endif

namespace Models {
    ZipArchiver::ZipArchiver() {
        m_ArchiveCreator = new QFutureWatcher<QStringList>(this);
        QObject::connect(m_ArchiveCreator, SIGNAL(resultReadyAt(int)), SLOT(archiveCreated(int)));
        QObject::connect(m_ArchiveCreator, SIGNAL(finished()), SLOT(allFinished()));
    }

    int ZipArchiver::getItemsCount() const {
        auto &snapshot = getArtworksSnapshot();
        const size_t size = snapshot.size();
        int count = 0;
        for (size_t i = 0; i < size; ++i) {
            ImageArtwork *image = dynamic_cast<ImageArtwork *>(snapshot.get(i));
            if (image != NULL && image->hasVectorAttached()) {
                count++;
            }
        }

        return count;
    }

    void ZipArchiver::archiveCreated(int) {
        incProgress();
    }

    void ZipArchiver::allFinished() {
        LOG_INFO << "#";
        endProcessing();
    }

    void ZipArchiver::archiveArtworks() {
        LOG_DEBUG << getItemsCount() << "item(s) pending";
        QHash<QString, QStringList> itemsWithSameName;
        fillFilenamesHash(itemsWithSameName);

        if (itemsWithSameName.empty()) {
            LOG_INFO << "No items to zip. Exiting...";
            endProcessing();
            return;
        }

        beginProcessing();
        restrictMaxThreads();

        QList<QStringList> items = itemsWithSameName.values();

        LOG_INFO << "Creating zip archives for" << items.length() << "item(s)";
#ifndef CORE_TESTS
        m_ArchiveCreator->setFuture(QtConcurrent::mapped(items, Helpers::zipFiles));
#endif
    }

    void ZipArchiver::fillFilenamesHash(QHash<QString, QStringList> &hash) {
        auto &snapshot = getArtworksSnapshot();
        auto &artworksList = snapshot.getWeakSnapshot();
        LOG_DEBUG << "Processing" << artworksList.size() << "item(s)";

        for (auto &artwork: artworksList) {
            const QString &filepath = artwork->getFilepath();

            QFileInfo fi(filepath);
            QString basename = fi.baseName();

            ImageArtwork *image = dynamic_cast<ImageArtwork*>(artwork);
            if (image != NULL) {
                if (image->hasVectorAttached()) {
                    LOG_INTEGRATION_TESTS << filepath << "is zipping candidate";
                    if (!hash.contains(basename)) {
                        hash.insert(basename, QStringList());
                    }

                    hash[basename].append(filepath);
                    hash[basename].append(image->getAttachedVectorPath());
                } else {
                    LOG_INTEGRATION_TESTS << filepath << "does not have vector attached";
                }
            } else {
                LOG_INTEGRATION_TESTS << filepath << "is not an image";
            }
        }
    }
}

