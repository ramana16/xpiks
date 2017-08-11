/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../Common/defines.h"
#include "../MetadataIO/metadataiocoordinator.h"
#include "launchexiftooljobitem.h"

namespace Maintenance {
    LaunchExiftoolJobItem::LaunchExiftoolJobItem(const QString &settingsExiftoolPath,
                                                 MetadataIO::MetadataIOCoordinator *coordinator):
        m_SettingsExiftoolPath(settingsExiftoolPath),
        m_MetadataIOCoordinator(coordinator)
    {
        Q_ASSERT(coordinator != nullptr);
    }

    void LaunchExiftoolJobItem::processJob() {
        LOG_DEBUG << "#";
        doLaunchExiftool();
    }

    void LaunchExiftoolJobItem::doLaunchExiftool() {
        m_MetadataIOCoordinator->tryToLaunchExiftool(m_SettingsExiftoolPath);
        m_MetadataIOCoordinator->exiftoolDiscoveryFinished();
    }
}
