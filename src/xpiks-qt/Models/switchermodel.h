/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SWITCHERMODEL_H
#define SWITCHERMODEL_H

#include <QObject>
#include <QTimer>
#include "../Connectivity/switcherconfig.h"
#include "../Helpers/localconfig.h"
#include "../Common/baseentity.h"
#include "../Common/statefulentity.h"

namespace Models {
    class SwitcherModel: public QObject, public Common::BaseEntity, public Common::StatefulEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isDonationCampaign1Active READ getIsDonationCampaign1On NOTIFY switchesUpdated)
        Q_PROPERTY(QString donateCampaign1Link READ getDonateCampaign1Link CONSTANT)
        Q_PROPERTY(bool isDonateCampaign1LinkClicked READ getDonateCampaign1LinkClicked NOTIFY donateCampaign1LinkClicked)
        Q_PROPERTY(bool isDonateCampaign1Stage2On READ getIsDonateCampaign1Stage2On NOTIFY switchesUpdated)
        Q_PROPERTY(bool useAutoImport READ getUseAutoImport NOTIFY switchesUpdated)
    public:
        SwitcherModel(QObject *parent=nullptr);

    public:
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;

    public:
        void initialize();
        void updateConfigs();
        void afterInitializedCallback();

    private:
        void ensureSessionTokenValid();

    public:
        bool getIsDonationCampaign1On() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DonateCampaign1, m_Threshold); }
        bool getIsDonateCampaign1Stage2On() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DonateCampaign1Stage2, m_Threshold); }
        bool getProgressiveSuggestionPreviewsOn() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::ProgressiveSuggestionPreviews, m_Threshold); }
        bool getUseDirectMetadataExport() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::DirectMetadataExport, m_Threshold); }
        bool getGoodQualityVideoPreviews() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::GoodQualityVideoPreviews, m_Threshold); }
        bool getUseAutoImport() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::MetadataAutoImport, m_Threshold); }
        bool getGettySuggestionEnabled() { return m_Config.isSwitchOn(Connectivity::SwitcherConfig::GettySuggestionEnabled, m_Threshold); }

    public:
        bool getDonateCampaign1LinkClicked() const;
        QString getDonateCampaign1Link() const { return QString("https://ribtoks.github.io/xpiks/donatecampaign/"); }

#ifdef INTEGRATION_TESTS
    public:
        void setRemoteConfigOverride(const QString &localPath) { m_Config.setRemoteOverride(localPath); }
#endif

    public:
        Q_INVOKABLE void setDonateCampaign1LinkClicked();

    private slots:
        void configUpdated();
        void onDelayTimer();

    signals:
        void switchesUpdated();
        void donateCampaign1LinkClicked();
        void donateDialogRequested();

    private:
        void readEngagementConfig();

    private:
        QTimer m_DelayTimer;
        Connectivity::SwitcherConfig m_Config;
        int m_Threshold;
    };
}

#endif // SWITCHERMODEL_H
