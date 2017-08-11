/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QThread>
#include "maintenanceworker.h"

#define MAINTENANCE_SLEEP 1

namespace Maintenance {
    bool MaintenanceWorker::initWorker() {
        LOG_DEBUG << "#";
        return true;
    }

    void MaintenanceWorker::processOneItem(std::shared_ptr<IMaintenanceItem> &item) {
        item->processJob();

        // make this thread more non-intrusive
        QThread::sleep(MAINTENANCE_SLEEP);
    }
}
