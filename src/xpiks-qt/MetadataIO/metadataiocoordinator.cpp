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
#include "../Models/filteredartitemsproxymodel.h"
#include "../QMLExtensions/videocachingservice.h"

namespace MetadataIO {
    void lockForIO(const MetadataIO::ArtworksSnapshot &snapshot) {
        const size_t size = snapshot.size();
        LOG_DEBUG << size << "item(s)";

        for (size_t i = 0; i < size; i++) {
            Models::ArtworkMetadata *artwork = snapshot.get(i);
            artwork->setIsLockedIO(true);
        }
    }

    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity(),
        m_LastImportID(1),
        m_ProcessingItemsCount(0),
        m_IsInProgress(false),
        m_HasErrors(false),
        m_ExiftoolNotFound(false)
    {
        LOG_INFO << "Supported image formats:" << QImageReader::supportedImageFormats();

        QObject::connect(&m_WritingAsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                         this, &MetadataIOCoordinator::writingWorkersFinished);

        QObject::connect(&m_ReadingHub, &MetadataReadingHub::readingFinished,
                         this, &MetadataIOCoordinator::onReadingFinished);
    }

    void MetadataIOCoordinator::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);
        m_ReadingHub.setCommandManager(commandManager);
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

    bool MetadataIOCoordinator::shouldUseAutoImport() const {
        bool autoImport = false;

#if !defined(CORE_TESTS)
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        Models::SwitcherModel *switcherModel = m_CommandManager->getSwitcherModel();
        if (settingsModel->getUseAutoImport() && switcherModel->getUseAutoImport()) {
            autoImport = true;
        }
#endif

        return autoImport;
    }

    int MetadataIOCoordinator::readMetadataExifTool(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        int importID = getNextImportID();
        initializeImport(artworksToRead, importID, storageReadBatchID);

        libxpks::io::ReadingOrchestrator readingOrchestrator(&m_ReadingHub,
                                                             m_CommandManager->getSettingsModel());
        readingOrchestrator.startReading();

        return importID;
    }

    void MetadataIOCoordinator::writeMetadataExifTool(const ArtworksSnapshot &artworksToWrite, bool useBackups) {
        LOG_DEBUG << "use backups:" << useBackups;
        m_WritingAsyncCoordinator.reset();

        lockForIO(artworksToWrite);

        // this should prevent a race between video thumbnails and exiftool
        // https://github.com/ribtoks/xpiks/issues/477
        // ---
        QMLExtensions::VideoCachingService *videoCachingService = m_CommandManager->getVideoCachingService();
        videoCachingService->waitWorkerIdle();
        // ---

        libxpks::io::WritingOrchestrator writingOrchestrator(artworksToWrite,
                                                             &m_WritingAsyncCoordinator,
                                                             m_CommandManager->getSettingsModel());

#ifndef INTEGRATION_TESTS
        auto *switcher = m_CommandManager->getSwitcherModel();
        const bool directExportOn = switcher->getUseDirectMetadataExport();
#else
        const bool directExportOn = false;
#endif

        auto *settingsModel = m_CommandManager->getSettingsModel();
        const bool useDirectExport = settingsModel->getUseDirectExiftoolExport();

        writingOrchestrator.startWriting(useBackups, useDirectExport || directExportOn);
    }

    void MetadataIOCoordinator::autoDiscoverExiftool() {
        LOG_DEBUG << "#";
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        QString existingExiftoolPath = settingsModel->getExifToolPath();
        Maintenance::MaintenanceService *maintenanceService = m_CommandManager->getMaintenanceService();
        maintenanceService->launchExiftool(existingExiftoolPath, this);
    }

    void MetadataIOCoordinator::continueReading(bool ignoreBackups) {
        LOG_DEBUG << "ignore backups:" << ignoreBackups;
        setIsInProgress(true);
        m_ReadingHub.proceedImport(ignoreBackups);
    }

    void MetadataIOCoordinator::continueWithoutReading() {
        LOG_DEBUG << "#";
        setIsInProgress(true);
        m_ReadingHub.cancelImport();
    }

    bool MetadataIOCoordinator::hasImportFinished(int importID) {
        Q_ASSERT(m_PreviousImportIDs.find(0) == m_PreviousImportIDs.end());
        if (importID == 0) { return false; }
        bool found = m_PreviousImportIDs.find(importID) != m_PreviousImportIDs.end();
        return found;
    }

    void MetadataIOCoordinator::writingWorkersFinished(int status) {
        LOG_DEBUG << status;

        Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
        filteredModel->updateSelectedArtworksEx(QVector<int>() << Models::ArtItemsModel::IsModifiedRole);

        Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
        artItemsModel->unlockAllForIO();

        emit metadataWritingFinished();
    }

    void MetadataIOCoordinator::onReadingFinished(int importID) {
        LOG_DEBUG << "import #" << importID;

        m_PreviousImportIDs.insert(importID);
        emit metadataReadingFinished();
        setIsInProgress(false);
    }

    int MetadataIOCoordinator::getNextImportID() {
        int id = m_LastImportID++;
        return id;
    }

    void MetadataIOCoordinator::initializeImport(const ArtworksSnapshot &artworksToRead, int importID, quint32 storageReadBatchID) {
        m_ReadingHub.initializeImport(artworksToRead, importID, storageReadBatchID);

        setHasErrors(false);
        setIsInProgress(false);
        setProcessingItemsCount((int)artworksToRead.size());
    }
}
