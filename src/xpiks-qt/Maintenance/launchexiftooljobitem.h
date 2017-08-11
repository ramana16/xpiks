/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#ifndef LAUNCHEXIFTOOLJOBITEM_H
#define LAUNCHEXIFTOOLJOBITEM_H

#include <QString>
#include "imaintenanceitem.h"

namespace MetadataIO {
    class MetadataIOCoordinator;
}

namespace Maintenance {
    class LaunchExiftoolJobItem : public IMaintenanceItem
    {
    public:
        LaunchExiftoolJobItem(const QString &settingsExiftoolPath, MetadataIO::MetadataIOCoordinator *coordinator);

    public:
        virtual void processJob() override;

    private:
        void doLaunchExiftool();

    private:
        const QString m_SettingsExiftoolPath;
        MetadataIO::MetadataIOCoordinator *m_MetadataIOCoordinator;
    };
}

#endif // LAUNCHEXIFTOOLJOBITEM_H
