/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINTENANCESERVICE_H
#define MAINTENANCESERVICE_H

#include <QObject>
#include "../MetadataIO/artworkmetadatasnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Suggestion {
    class LocalLibrary;
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
        void addArtworksToLibrary(std::unique_ptr<MetadataIO::ArtworksSnapshot> &artworksSnapshot, Suggestion::LocalLibrary *localLibrary);
        void cleanupLogs();
        void loadLocalLibrary(Suggestion::LocalLibrary *localLibrary);
        void saveLocalLibrary(Suggestion::LocalLibrary *localLibrary);
        void cleanupLocalLibrary(Suggestion::LocalLibrary *localLibrary);
        void moveSettings(Models::SettingsModel *settingsModel);
        void saveSession(std::unique_ptr<MetadataIO::SessionSnapshot> &sessionSnapshot, Models::SessionManager *sessionManager);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject *object);

    private:
        MaintenanceWorker *m_MaintenanceWorker;
    };
}

#endif // MAINTENANCESERVICE_H
