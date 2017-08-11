/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WARNINGSSERVICE_H
#define WARNINGSSERVICE_H

#include <QObject>
#include "../Common/baseentity.h"
#include "../Common/iservicebase.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"
#include "warningssettingsmodel.h"

namespace Warnings {
    class WarningsCheckingWorker;

    class WarningsService:
        public QObject,
        public Common::BaseEntity,
        public Common::IServiceBase<Models::ArtworkMetadata, Common::WarningsCheckFlags>
    {
    Q_OBJECT

    public:
        explicit WarningsService(QObject *parent=0);
        virtual ~WarningsService() {}

    public:
        void initWarningsSettings();
        const WarningsSettingsModel *getWarningsSettingsModel() const { return &m_WarningsSettingsModel; }
        void cancelCurrentBatch();

    public:
        virtual void startService(const std::shared_ptr<Common::ServiceStartParams> &params) override;
        virtual void stopService() override;

        virtual bool isAvailable() const override { return true; }
        virtual bool isBusy() const override;

        virtual void submitItem(Models::ArtworkMetadata *item) override;
        virtual void submitItem(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags) override;
        virtual void submitItems(const QVector<Models::ArtworkMetadata *> &items) override;
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;

    private slots:
        void workerDestoyed(QObject *object);
        void workerStopped();
        void updateWarningsSettings();

    signals:
        void queueIsEmpty();

    private:
        WarningsCheckingWorker *m_WarningsWorker;
        WarningsSettingsModel m_WarningsSettingsModel;
    };
}

#endif // WARNINGSSERVICE_H
