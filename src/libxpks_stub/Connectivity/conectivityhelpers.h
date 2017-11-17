/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CONECTIVITYHELPERS_H
#define CONECTIVITYHELPERS_H

#include <QStringList>
#include <QVector>
#include <memory>
#include <vector>
#include "uploadcontext.h"

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
    class ProxySettings;
    class SettingsModel;
}

namespace Encryption {
    class SecretsManager;
}

namespace MetadataIO {
    class ArtworksSnapshot;
}

namespace libxpks {
    namespace net {
        class UploadBatch;

        void extractFilePathes(const MetadataIO::ArtworksSnapshot &artworksSnapshot,
                               QStringList &filePathes,
                               QStringList &zipsPathes);

        void generateUploadContexts(const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                    std::vector<std::shared_ptr<UploadContext> > &contexts,
                                    Encryption::SecretsManager *secretsManager,
                                    Models::SettingsModel *settingsModel);

        std::vector<std::shared_ptr<UploadBatch> > generateUploadBatches(const MetadataIO::ArtworksSnapshot &artworksToUpload,
                                                                         const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                                                         Encryption::SecretsManager *secretsManager,
                                                                         Models::SettingsModel *settingsModel);
    }
}

#endif // CONECTIVITYHELPERS_H
