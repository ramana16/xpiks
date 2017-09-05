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

namespace MetadataIO {
    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity(),
        m_ProcessingItemsCount(0),
        m_HasErrors(false),
        m_ExiftoolNotFound(false)
    {
        LOG_INFO << "Supported image formats:" << QImageReader::supportedImageFormats();        

        QObject::connect(&m_WritingAsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                         this, &MetadataIOCoordinator::writingWorkersFinished);

        QObject::connect(&m_ReadingHub, &MetadataReadingHub::readingFinished,
                         this, &MetadataIOCoordinator::metadataReadingFinished);
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

    void MetadataIOCoordinator::readMetadataExifTool(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        initializeImport(artworksToRead, storageReadBatchID);

        libxpks::io::ReadingOrchestrator readingOrchestrator(&m_ReadingHub,
                                                             m_CommandManager->getSettingsModel());
        readingOrchestrator.startReading();
    }

    void MetadataIOCoordinator::writeMetadataExifTool(const ArtworksSnapshot &artworksToWrite, bool useBackups) {
        m_WritingAsyncCoordinator.reset();

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
        m_ReadingHub.proceedImport(ignoreBackups);
    }

    void MetadataIOCoordinator::continueWithoutReading() {
        m_ReadingHub.cancelImport();
    }

    void MetadataIOCoordinator::writingWorkersFinished(int status) {
        LOG_DEBUG << status;
        Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
        filteredModel->updateSelectedArtworksEx(QVector<int>() << Models::ArtItemsModel::IsModifiedRole);
        emit metadataWritingFinished();
    }

    void MetadataIOCoordinator::initializeImport(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        m_ReadingHub.initializeImport(artworksToRead, storageReadBatchID);
        setHasErrors(false);
        setProcessingItemsCount((int)artworksToRead.size());
    }
}
