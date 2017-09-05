/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadataiocoordinator.h"
#include <QHash>
#include <QFileInfo>
#include <QProcess>
#include <QImageReader>
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"
#include "../Models/imageartwork.h"
#include "../Maintenance/maintenanceservice.h"
#include "../Warnings/warningsmodel.h"
#include "metadataioservice.h"
#include <readingorchestrator.h>
#include <writingorchestrator.h>
#include "../Models/switchermodel.h"
#include "../QMLExtensions/artworksupdatehub.h"

namespace MetadataIO {
    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity(),
        m_ProcessingItemsCount(0),
        m_IsImportInProgress(false),
        m_CanProcessResults(false),
        m_HasErrors(false),
        m_ExiftoolNotFound(false)
    {
        LOG_INFO << "Supported image formats:" << QImageReader::supportedImageFormats();
    }

    void MetadataIOCoordinator::readingFinished(bool success) {
        LOG_INFO << "Success:" << success;
        setHasErrors(!success);

        if (m_CanProcessResults) {
            readingFinishedHandler(m_IgnoreBackupsAtImport);
        } else {
            LOG_INFO << "Can't process results. Waiting for user interaction...";
        }

        m_IsImportInProgress = false;
    }

    void MetadataIOCoordinator::writingFinished(bool success) {
        LOG_INFO << success;
        setHasErrors(!success);
        emit metadataWritingFinished();
    }

    void MetadataIOCoordinator::setRecommendedExiftoolPath(const QString &recommendedExiftool) {
        LOG_DEBUG << recommendedExiftool;
        setExiftoolNotFound(recommendedExiftool.isEmpty());
        m_RecommendedExiftoolPath = recommendedExiftool;

        if (!m_ExiftoolNotFound && !m_RecommendedExiftoolPath.isEmpty()) {
            LOG_DEBUG << "Recommended exiftool path is" << m_RecommendedExiftoolPath;

            Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
            QString existingExiftoolPath = settingsModel->getExifToolPath();

            if (existingExiftoolPath != m_RecommendedExiftoolPath) {
                LOG_INFO << "Setting exiftool path to recommended";
                settingsModel->setExifToolPath(m_RecommendedExiftoolPath);
                settingsModel->saveExiftool();
            }
        }
    }

    void MetadataIOCoordinator::readMetadataExifTool(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        initializeImport(artworksToRead, storageReadBatchID);

        auto *readingOrchestrator = new libxpks::io::ReadingOrchestrator(artworksToRead,
                                                                         &m_ReadingHub,
                                                                         m_CommandManager->getSettingsModel(),
                                                                         storageReadBatchID);
        readingOrchestrator->startReading();
    }

    void MetadataIOCoordinator::writeMetadataExifTool(const ArtworksSnapshot &artworksToWrite, bool useBackups) {
        libxpks::io::WritingOrchestrator *writingOrchestrator = new libxpks::io::WritingOrchestrator(artworksToWrite,
                                                                                                     m_CommandManager->getSettingsModel());

        QObject::connect(writingOrchestrator, &libxpks::io::WritingOrchestrator::allFinished, this, &MetadataIOCoordinator::writingFinished);
        QObject::connect(this, &MetadataIOCoordinator::metadataWritingFinished, writingOrchestrator, &libxpks::io::WritingOrchestrator::deleteLater);
        m_WritingWorker = writingOrchestrator;

#ifndef INTEGRATION_TESTS
        auto *switcher = m_CommandManager->getSwitcherModel();
        const bool directExportOn = switcher->getUseDirectMetadataExport();
#else
        const bool directExportOn = false;
#endif

        auto *settingsModel = m_CommandManager->getSettingsModel();
        const bool useDirectExport = settingsModel->getUseDirectExiftoolExport();

        writingOrchestrator->startWriting(useBackups, useDirectExport || directExportOn);
    }

    void MetadataIOCoordinator::autoDiscoverExiftool() {
        LOG_DEBUG << "#";
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        QString existingExiftoolPath = settingsModel->getExifToolPath();
        Maintenance::MaintenanceService *maintenanceService = m_CommandManager->getMaintenanceService();
        maintenanceService->launchExiftool(existingExiftoolPath, this);
    }

    void MetadataIOCoordinator::continueReading(bool ignoreBackups) {
        m_ReadingHub.setIgnoreBackups(ignoreBackups);
        m_ReadingHub.proceedImport();
    }

    void MetadataIOCoordinator::continueWithoutReading() {
        m_ReadingHub.setInitAsEmpty();
        m_ReadingHub.cancelImport();
    }

    void MetadataIOCoordinator::initializeImport(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        m_ReadingHub.initializeImport(artworksToRead, storageReadBatchID);
        setHasErrors(false);
        setProcessingItemsCount((int)artworksToRead.size());
    }

    void MetadataIOCoordinator::readingFinishedHandler(bool ignoreBackups) {
        Q_ASSERT(m_CanProcessResults);
        Q_ASSERT(m_ReadingWorker != nullptr);
        m_CanProcessResults = false;

        LOG_DEBUG << "Setting imported metadata. Ignore backups:" << ignoreBackups;

        auto &importResults = m_ReadingWorker->getImportResults();
        auto &snapshot = m_ReadingWorker->getArtworksSnapshot();

        const bool shouldOverwrite = ignoreBackups;
        auto &items = snapshot.getRawData();
        LOG_INFO << "Initializing" << items.size() << "artwork(s)";
        for (auto &item: items) {
            Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
            const QString &filepath = artwork->getFilepath();

            for (const auto &importResult: importResults) {
                if (importResult.contains(filepath)) {
                    const OriginalMetadata &importResultItem = importResult.value(filepath);
                    artwork->initFromOrigin(importResultItem, shouldOverwrite);
                    break;
                }
            }
        }

        afterImportHandler(snapshot.getWeakSnapshot(), ignoreBackups);

        emit metadataReadingFinished();
        LOG_DEBUG << "Metadata import finished";
    }

    void MetadataIOCoordinator::afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups) {
        if (!getHasErrors()) {
            m_CommandManager->addToLibrary(itemsToRead);
        }

        Q_UNUSED(ignoreBackups);

        m_CommandManager->updateArtworks(itemsToRead);
        m_CommandManager->submitForSpellCheck(itemsToRead);
        m_CommandManager->submitForWarningsCheck(itemsToRead);
    }
}
