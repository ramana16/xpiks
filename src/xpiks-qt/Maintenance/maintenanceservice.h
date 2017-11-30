/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MAINTENANCESERVICE_H
#define MAINTENANCESERVICE_H

#include <QObject>
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class MetadataIOCoordinator;
}

namespace Translation {
    class TranslationManager;
}

namespace Models {
    class SettingsModel;
    class SessionManager;
}

namespace Helpers {
    class AsyncCoordinator;
}

namespace QMLExtensions {
    class ImageCachingService;
}

namespace Maintenance {
    class MaintenanceWorker;

    class MaintenanceService: public QObject
    {
        Q_OBJECT
    public:
        MaintenanceService();

    public:
        void startService();
        void stopService();

    public:
        void cleanupUpdatesArtifacts();
        void launchExiftool(const QString &settingsExiftoolPath, MetadataIO::MetadataIOCoordinator *coordinator);
        void initializeDictionaries(Translation::TranslationManager *translationManager, Helpers::AsyncCoordinator *initCoordinator);
        void cleanupLogs();
        void moveSettings(Models::SettingsModel *settingsModel);
        void upgradeImagesCache(QMLExtensions::ImageCachingService *imageCachingService);
        void saveSession(std::unique_ptr<MetadataIO::SessionSnapshot> &sessionSnapshot, Models::SessionManager *sessionManager);
        void cleanupOldXpksBackups(const QString &directory);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject *object);

    private:
        MaintenanceWorker *m_MaintenanceWorker;
    };
}

#endif // MAINTENANCESERVICE_H
