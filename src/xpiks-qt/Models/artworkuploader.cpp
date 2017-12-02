/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworkuploader.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QQmlEngine>
#include "uploadinforepository.h"
#include "uploadinfo.h"
#include "../Common/defines.h"
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/filehelpers.h"
#include "../Connectivity/iftpcoordinator.h"
#include "../Connectivity/testconnection.h"
#include <uploadcontext.h>
#include "../Models/imageartwork.h"
#include "../Connectivity/ftphelpers.h"
#include "../Helpers/asynccoordinator.h"

#ifndef CORE_TESTS
#include <ftpcoordinator.h>
#endif

namespace Models {
    ArtworkUploader::ArtworkUploader(Connectivity::IFtpCoordinator *ftpCoordinator, QObject *parent):
        QObject(parent),
        m_FtpCoordinator(ftpCoordinator),
        m_Percent(0),
        m_IsInProgress(false),
        m_HasErrors(false)
    {
        libxpks::net::FtpCoordinator *coordinator = dynamic_cast<libxpks::net::FtpCoordinator *>(ftpCoordinator);
        Q_ASSERT(coordinator != nullptr);
        QObject::connect(coordinator, &libxpks::net::FtpCoordinator::uploadStarted, this, &ArtworkUploader::onUploadStarted);
        QObject::connect(coordinator, &libxpks::net::FtpCoordinator::uploadFinished, this, &ArtworkUploader::allFinished);
        QObject::connect(coordinator, &libxpks::net::FtpCoordinator::overallProgressChanged, this, &ArtworkUploader::uploaderPercentChanged);

        m_TestingCredentialWatcher = new QFutureWatcher<Connectivity::ContextValidationResult>(this);
        QObject::connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
        QObject::connect(coordinator, &libxpks::net::FtpCoordinator::transferFailed,
                         &m_UploadWatcher, &Connectivity::UploadWatcher::reportUploadErrorHandler);

        QObject::connect(&m_StocksFtpList, &AutoComplete::StocksFtpListModel::stocksListUpdated, this, &ArtworkUploader::stocksListUpdated);
    }

    ArtworkUploader::~ArtworkUploader() {
        delete m_TestingCredentialWatcher;

        if (m_FtpCoordinator != NULL) {
            delete m_FtpCoordinator;
        }
    }

    void ArtworkUploader::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);

        libxpks::net::FtpCoordinator *coordinator = dynamic_cast<libxpks::net::FtpCoordinator *>(m_FtpCoordinator);
        Q_ASSERT(coordinator != NULL);
        coordinator->setCommandManager(commandManager);

        m_StocksFtpList.setCommandManager(commandManager);
    }

    void ArtworkUploader::setPercent(int value) {
        if (m_Percent != value) {
            m_Percent = value;
            emit percentChanged();
        }
    }

    void ArtworkUploader::setInProgress(bool value) {
        if (m_IsInProgress != value) {
            m_IsInProgress = value;
            emit inProgressChanged();
        }
    }

    void ArtworkUploader::setHasErrors(bool value) {
        if (m_HasErrors != value) {
            m_HasErrors = value;
            emit hasErrorsChanged();
        }
    }

    void ArtworkUploader::onUploadStarted() {
        LOG_DEBUG << "#";
        setHasErrors(false);
        setInProgress(true);
        setPercent(0);
        emit startedProcessing();
    }

    void ArtworkUploader::allFinished(bool anyError) {
        LOG_INFO << "anyError =" << anyError;
        setHasErrors(anyError);
        setPercent(100);
        setInProgress(false);
        emit finishedProcessing();
    }

    void ArtworkUploader::credentialsTestingFinished() {
        Connectivity::ContextValidationResult result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result.m_Result, result.m_Host);
    }

    void ArtworkUploader::uploaderPercentChanged(double percent) {
        int percentage = percent < 1.0 ? 1.0 : percent;
        setPercent((int)percentage);
        LOG_DEBUG << "Overall progress =" << percent;

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        uploadInfoRepository->updatePercentages();
    }

    void ArtworkUploader::stocksListUpdated() {
        LOG_DEBUG << "#";

        QStringList stocks = m_StocksFtpList.getStockNamesList();
        m_StocksCompletionSource.setStrings(stocks);
    }

    void ArtworkUploader::updateStocksList() {
        LOG_DEBUG << "#";
        m_StocksFtpList.initializeConfigs();
    }

    void ArtworkUploader::uploadArtworks() {
        doUploadArtworks(m_ArtworksSnapshot);
    }

    void ArtworkUploader::checkCredentials(const QString &host, const QString &username,
                                           const QString &password, bool disablePassiveMode, bool disableEPSV) const {
        std::shared_ptr<libxpks::net::UploadContext> context(new libxpks::net::UploadContext());

        context->m_Host = host;
        context->m_Username = username;
        context->m_Password = password;
        context->m_TimeoutSeconds = 10;
        context->m_UsePassiveMode = !disablePassiveMode;
        context->m_UseEPSV = !disableEPSV;

        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        context->m_UseProxy = settingsModel->getUseProxy();
        context->m_ProxySettings = settingsModel->getProxySettings();
        context->m_VerboseLogging = settingsModel->getVerboseUpload();

        m_TestingCredentialWatcher->setFuture(QtConcurrent::run(Connectivity::isContextValid, context));
    }

    bool ArtworkUploader::needCreateArchives() const {
        bool anyZipNeeded = false;
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        auto &infos = uploadInfoRepository->getUploadInfos();

        for (auto &info: infos) {
            if (info->getIsSelected() && info->getZipBeforeUpload()) {
                anyZipNeeded = true;
                LOG_DEBUG << "at least for" << info->getHost();
                break;
            }
        }

        bool needCreate = false;

        if (anyZipNeeded) {
            auto &snapshot = this->getArtworksSnapshot();
            auto &artworkList = snapshot.getWeakSnapshot();
            for (auto *artwork: artworkList) {
                ImageArtwork *image = dynamic_cast<ImageArtwork *>(artwork);

                if (image == NULL || !image->hasVectorAttached()) {
                    continue;
                }

                const QString &filepath = artwork->getFilepath();
                QString archivePath = Helpers::getArchivePath(filepath);
                QFileInfo fi(archivePath);

                if (!fi.exists()) {
                    needCreate = true;
                    LOG_DEBUG << "Zip needed at least for" << archivePath;
                    break;
                }
            }
        }

        return needCreate;
    }

    void ArtworkUploader::initializeStocksList(Helpers::AsyncCoordinator *initCoordinator) {
        LOG_DEBUG << "#";

        Helpers::AsyncCoordinatorLocker locker(initCoordinator);
        Q_UNUSED(locker);
        Helpers::AsyncCoordinatorUnlocker unlocker(initCoordinator);
        Q_UNUSED(unlocker);

        updateStocksList();
    }

    void ArtworkUploader::resetModel() {
        LOG_DEBUG << "#";
        clearModel();
        resetArtworks();
    }

    void ArtworkUploader::clearModel() {
        LOG_DEBUG << "#";
        resetProgress();
        setHasErrors(false);
        m_UploadWatcher.resetModel();
    }

    void ArtworkUploader::resetProgress() {
        LOG_DEBUG << "#";
        setPercent(0);
        setInProgress(false);
    }

    void ArtworkUploader::cancelOperation() {
        LOG_DEBUG << "#";
        m_FtpCoordinator->cancelUpload();
    }

    void ArtworkUploader::setArtworks(MetadataIO::ArtworksSnapshot &snapshot) {
        LOG_DEBUG << "#";
        m_ArtworksSnapshot = std::move(snapshot);
        emit itemsCountChanged();
    }

    void ArtworkUploader::resetArtworks() {
        LOG_DEBUG << "#";
        m_ArtworksSnapshot.clear();
        emit itemsCountChanged();
    }

    void ArtworkUploader::doUploadArtworks(const MetadataIO::ArtworksSnapshot &snapshot) {
        LOG_INFO << snapshot.size() << "artwork(s)";

        if (snapshot.empty()) { return; }

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        std::vector<std::shared_ptr<Models::UploadInfo> > selectedInfos = std::move(uploadInfoRepository->retrieveSelectedUploadInfos());

        uploadInfoRepository->resetPercents();
        uploadInfoRepository->updatePercentages();

        m_FtpCoordinator->uploadArtworks(snapshot, selectedInfos);
        xpiks()->reportUserAction(Connectivity::UserAction::Upload);
    }

    bool ArtworkUploader::removeUnavailableItems() {
        LOG_DEBUG << "#";

        auto &artworksListOld = getArtworksSnapshot();
        MetadataIO::ArtworksSnapshot::Container artworksListNew;

        const size_t size = artworksListOld.size();
        for (size_t i = 0; i < size; ++i) {
            auto &item = artworksListOld.at(i);

            if (!item->getArtworkMetadata()->isUnavailable()) {
                artworksListNew.push_back(item);
            }
        }

        bool anyUnavailable = artworksListNew.size() != m_ArtworksSnapshot.size();
        if (anyUnavailable) {
            m_ArtworksSnapshot.set(artworksListNew);

            if (m_ArtworksSnapshot.empty()) {
                emit requestCloseWindow();
            }

            emit itemsCountChanged();
        }

        return anyUnavailable;
    }

    QString ArtworkUploader::getFtpName(const QString &stockAddress) const {
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        auto &infos = uploadInfoRepository->getUploadInfos();

        for (auto &info: infos) {
            if (Connectivity::sanitizeHost(info->getHost()) == stockAddress) {
                return info->getTitle();
            }
        }

        return QString();
    }

    QObject *ArtworkUploader::getUploadWatcher() {
        auto *model = &m_UploadWatcher;
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);

        return model;
    }
}
