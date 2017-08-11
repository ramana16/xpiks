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
#include "uploadinforepository.h"
#include "uploadinfo.h"
#include "../Common/defines.h"
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/filenameshelpers.h"
#include "../Conectivity/iftpcoordinator.h"
#include "../Conectivity/testconnection.h"
#include "../Conectivity/uploadcontext.h"
#include "../Models/imageartwork.h"
#include "../Conectivity/ftphelpers.h"
#include "../Helpers/asynccoordinator.h"

#ifndef CORE_TESTS
#include "../Conectivity/ftpcoordinator.h"
#endif

namespace Models {
    ArtworkUploader::ArtworkUploader(Conectivity::IFtpCoordinator *ftpCoordinator, QObject *parent):
        ArtworksProcessor(parent),
        m_FtpCoordinator(ftpCoordinator),
        m_Percent(0) {
        Conectivity::FtpCoordinator *coordinator = dynamic_cast<Conectivity::FtpCoordinator *>(ftpCoordinator);
        QObject::connect(coordinator, &Conectivity::FtpCoordinator::uploadStarted, this, &ArtworkUploader::onUploadStarted);
        QObject::connect(coordinator, &Conectivity::FtpCoordinator::uploadFinished, this, &ArtworkUploader::allFinished);
        QObject::connect(coordinator, &Conectivity::FtpCoordinator::overallProgressChanged, this, &ArtworkUploader::uploaderPercentChanged);

        m_TestingCredentialWatcher = new QFutureWatcher<Conectivity::ContextValidationResult>(this);
        QObject::connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
        QObject::connect(coordinator, &Conectivity::FtpCoordinator::transferFailed,
                         &m_UploadWatcher, &Conectivity::UploadWatcher::reportUploadErrorHandler);

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

        Conectivity::FtpCoordinator *coordinator = dynamic_cast<Conectivity::FtpCoordinator *>(m_FtpCoordinator);
        Q_ASSERT(coordinator != NULL);
        coordinator->setCommandManager(commandManager);

        m_StocksFtpList.setCommandManager(commandManager);
    }

    void ArtworkUploader::onUploadStarted() {
        LOG_DEBUG << "#";
        beginProcessing();
        m_Percent = 0;
        updateProgress();
    }

    void ArtworkUploader::allFinished(bool anyError) {
        LOG_INFO << "anyError =" << anyError;
        setIsError(anyError);
        endProcessing();
        m_Percent = 100;
        updateProgress();
    }

    void ArtworkUploader::credentialsTestingFinished() {
        Conectivity::ContextValidationResult result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result.m_Result, result.m_Host);
    }

    void ArtworkUploader::uploaderPercentChanged(double percent) {
        m_Percent = (int)(percent);
        LOG_DEBUG << "Overall progress =" << percent;
        updateProgress();

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        uploadInfoRepository->updatePercentages();
    }

    void ArtworkUploader::stocksListUpdated() {
        LOG_DEBUG << "#";

        QStringList stocks = m_StocksFtpList.getStockNamesList();
        m_StocksCompletionSource.setStrings(stocks);
    }

    void ArtworkUploader::updateStocksList() {
        m_StocksFtpList.initializeConfigs();
    }

    void ArtworkUploader::uploadArtworks() { doUploadArtworks(getArtworkList()); }

    void ArtworkUploader::checkCredentials(const QString &host, const QString &username,
                                           const QString &password, bool disablePassiveMode, bool disableEPSV) const {
        Conectivity::UploadContext *context = new Conectivity::UploadContext();

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

        m_TestingCredentialWatcher->setFuture(QtConcurrent::run(Conectivity::isContextValid, context));
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
            const QVector<ArtworkMetadata *> &artworkList = this->getArtworkList();
            foreach(ArtworkMetadata *metadata, artworkList) {
                ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);

                if (image == NULL || !image->hasVectorAttached()) {
                    continue;
                }

                const QString &filepath = metadata->getFilepath();
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
        Helpers::AsyncCoordinatorLocker locker(initCoordinator);
        Q_UNUSED(locker);
        Helpers::AsyncCoordinatorUnlocker unlocker(initCoordinator);
        Q_UNUSED(unlocker);

        updateStocksList();
    }

    void ArtworkUploader::doUploadArtworks(const QVector<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();

        if (artworksCount == 0) {
            return;
        }

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        std::vector<std::shared_ptr<Models::UploadInfo> > selectedInfos = std::move(uploadInfoRepository->retrieveSelectedUploadInfos());

        uploadInfoRepository->resetPercents();
        uploadInfoRepository->updatePercentages();

        m_FtpCoordinator->uploadArtworks(artworkList, selectedInfos);
        m_CommandManager->reportUserAction(Conectivity::UserAction::Upload);
    }

    void ArtworkUploader::cancelProcessing() {
        m_FtpCoordinator->cancelUpload();
    }

    QString ArtworkUploader::getFtpName(const QString &stockAddress) const {
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        auto &infos = uploadInfoRepository->getUploadInfos();

        for (auto &info: infos) {
            if (Conectivity::sanitizeHost(info->getHost()) == stockAddress) {
                return info->getTitle();
            }
        }

        return QString();
    }
}
