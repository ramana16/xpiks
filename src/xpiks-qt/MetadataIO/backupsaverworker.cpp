/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "backupsaverworker.h"
#include <QFile>
#include "../Helpers/constants.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"

namespace MetadataIO {
    bool BackupSaverWorker::initWorker() {
        LOG_DEBUG << "#";
        return true;
    }

    void BackupSaverWorker::processOneItem(std::shared_ptr<SaverWorkerJobItem> &item) {
        Models::ArtworkMetadata *metadata = item->getArtworkMetadata();
        MetadataSavingCopy copy(metadata->getBasicModel());
        copy.saveToFile(metadata->getFilepath());
    }
}
