/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IFTPCOORDINATOR_H
#define IFTPCOORDINATOR_H

#include <vector>
#include <memory>
#include <QVector>

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
}

namespace MetadataIO {
    class ArtworksSnapshot;
}

namespace Connectivity {
    class IFtpCoordinator {
    public:
        virtual ~IFtpCoordinator() {}

        virtual void uploadArtworks(const MetadataIO::ArtworksSnapshot &artworksToUpload,
                            std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) = 0;
        virtual void cancelUpload() = 0;
    };
}

#endif // IFTPCOORDINATOR_H
