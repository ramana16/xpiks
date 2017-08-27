/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <QObject>

namespace Models {
    class SettingsModel;
}

namespace Connectivity {
    class UpdatesCheckerWorker;

    class UpdateService : public QObject
    {
        Q_OBJECT
    public:
        UpdateService(Models::SettingsModel *settingsModel);

    public:
        void startChecking();
        void stopChecking();

    private:
        void doStartChecking();
        void updateSettings();

    private slots:
        void workerFinished();
        void updateDownloadedHandler(const QString &updatePath, int version);

    signals:
        void updateAvailable(QString updateLink);
        void updateDownloaded(QString pathToUpdate);
        void cancelRequested();

    private:
        void saveUpdateInfo() const;

    private:
        Connectivity::UpdatesCheckerWorker *m_UpdatesCheckerWorker;
        Models::SettingsModel *m_SettingsModel;
        QString m_PathToUpdate;
        int m_AvailableVersion;
        volatile bool m_UpdateAvailable;
    };
}

Q_DECLARE_METATYPE(Connectivity::UpdateService*)

#endif // UPDATESERVICE_H
