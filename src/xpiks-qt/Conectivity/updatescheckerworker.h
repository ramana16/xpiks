/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UPDATESCHECKERWORKER_H
#define UPDATESCHECKERWORKER_H

#include <QObject>

namespace Models {
    class SettingsModel;
}

namespace Conectivity {
    struct UpdateCheckResult {
        QString m_UpdateURL;
        QString m_Checksum;
        int m_Version;
    };

    class UpdatesCheckerWorker : public QObject
    {
        Q_OBJECT
    public:
        UpdatesCheckerWorker(Models::SettingsModel *settingsModel, const QString &availableUpdatePath);
        virtual ~UpdatesCheckerWorker();

    private:
        void initWorker();
        void processOneItem();
        bool checkForUpdates(UpdateCheckResult &result);
        bool downloadUpdate(const UpdateCheckResult &updateCheckResult, QString &pathToUpdate);
        bool checkAvailableUpdate(const UpdateCheckResult &updateCheckResult);

    public slots:
        void process();

    signals:
        void stopped();
        void updateAvailable(QString updateLink);
        void updateDownloaded(const QString &updatePath, int version);
        void requestFinished();
        void cancelRequested();

    private:
        Models::SettingsModel *m_SettingsModel;
        QString m_UpdatesDirectory;
        QString m_AvailableUpdatePath;
    };
}

#endif // UPDATESCHECKERWORKER_H
