/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "conectivityhelpers.h"
#include <memory>
#include <QVector>
#include <Models/artworkmetadata.h>
#include <Models/uploadinfo.h>
#include <Encryption/secretsmanager.h>
#include "uploadcontext.h"
#include "uploadbatch.h"
#include <Helpers/filehelpers.h>
#include <Models/imageartwork.h>
#include <Commands/commandmanager.h>
#include <Models/settingsmodel.h>
#include <Models/proxysettings.h>
#include <MetadataIO/artworkssnapshot.h>

#define RETRIES_COUNT 3

namespace libxpks {
    namespace net {
        void extractFilePathes(const MetadataIO::ArtworksSnapshot &artworksSnapshot,
                               QStringList &filePathes,
                               QStringList &zipsPathes) {

            size_t size = artworksSnapshot.size();
            filePathes.reserve(size);
            zipsPathes.reserve(size);
            LOG_DEBUG << "Generating filepathes for" << size << "item(s)";

            for (size_t i = 0; i < size; ++i) {
                Models::ArtworkMetadata *metadata = artworksSnapshot.get(i);
                QString filepath = metadata->getFilepath();
                filePathes.append(filepath);

                Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

                if (image != NULL && image->hasVectorAttached()) {
                    filePathes.append(image->getAttachedVectorPath());

                    QString zipPath = Helpers::getArchivePath(filepath);
                    zipsPathes.append(zipPath);
                } else {
                    zipsPathes.append(filepath);
                }
            }
        }

        void generateUploadContexts(const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                    std::vector<std::shared_ptr<UploadContext> > &contexts,
                                    Encryption::SecretsManager *secretsManager,
                                    Models::SettingsModel *settingsModel) {
            size_t size = uploadInfos.size();
            contexts.reserve(size);

            Models::ProxySettings *proxySettings = settingsModel->getProxySettings();
            int timeoutSeconds = settingsModel->getUploadTimeout();
            bool useProxy = settingsModel->getUseProxy();

            for (size_t i = 0; i < size; ++i) {
                std::shared_ptr<UploadContext> context(new UploadContext());
                auto &info = uploadInfos.at(i);

                context->m_Host = info->getHost();
                context->m_Username = info->getUsername();
                context->m_Password = secretsManager->decodePassword(info->getPassword());
                context->m_UsePassiveMode = !info->getDisableFtpPassiveMode();
                context->m_UseEPSV = !info->getDisableEPSV();
                context->m_UseProxy = useProxy;
                context->m_ProxySettings = proxySettings;
                context->m_TimeoutSeconds = timeoutSeconds;
                // TODO: move to configs/options
                context->m_RetriesCount = RETRIES_COUNT;

                if (context->m_Host.contains("dreamstime")) {
                    context->m_DirForVectors = "additional";
                }

                if (context->m_Host.contains("alamy")) {
                    context->m_DirForImages = "Stock";
                    context->m_DirForVectors = "Vector";
                }

                contexts.emplace_back(context);
            }
        }

        std::vector<std::shared_ptr<UploadBatch> > generateUploadBatches(const MetadataIO::ArtworksSnapshot &artworksToUpload,
                                                                         const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                                                         Encryption::SecretsManager *secretsManager,
                                                                         Models::SettingsModel *settingsModel) {
            LOG_DEBUG << artworksToUpload.size() << "file(s)";
            std::vector<std::shared_ptr<UploadBatch> > batches;

            QStringList filePathes;
            QStringList zipFilePathes;
            extractFilePathes(artworksToUpload, filePathes, zipFilePathes);

            std::vector<std::shared_ptr<UploadContext> > contexts;
            generateUploadContexts(uploadInfos, contexts, secretsManager, settingsModel);

            size_t size = contexts.size();
            batches.reserve(size);

            for (size_t i = 0; i < size; ++i) {
                auto &context = contexts.at(i);

                if (uploadInfos[i]->getZipBeforeUpload()) {
                    batches.emplace_back(new UploadBatch(context, zipFilePathes));
                } else {
                    batches.emplace_back(new UploadBatch(context, filePathes));
                }
            }

            return batches;
        }
    }
}
