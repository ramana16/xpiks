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
#include "../Conectivity/switcherconfig.h"
#include "../Helpers/localconfig.h"
#include "../Common/baseentity.h"

namespace Models {
    class SwitcherModel: public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isDonationCampaign1Active READ getIsDonationCampaign1On NOTIFY switchesUpdated)
        Q_PROPERTY(QString donateCampaign1Link READ getDonateCampaign1Link CONSTANT)
        Q_PROPERTY(bool isDonateCampaign1LinkClicked READ getDonateCampaign1LinkClicked NOTIFY donateCampaign1LinkClicked)
        Q_PROPERTY(bool isDonateCampaign1Stage2On READ getIsDonateCampaign1Stage2On NOTIFY switchesUpdated)
    public:
        SwitcherModel(QObject *parent=nullptr);

    public:
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;
        void initEngagement();
        void updateConfigs();
        void afterInitializedCallback();

    public:
        bool getIsDonationCampaign1On() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::DonateCampaign1); }
        bool getIsDonateCampaign1Stage2On() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::DonateCampaign1Stage2); }
        bool getProgressiveSuggestionPreviewsOn() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::ProgressiveSuggestionPreviews); }

    public:
        bool getDonateCampaign1LinkClicked() const { return m_DonateCampaign1LinkClicked; }
        QString getDonateCampaign1Link() const { return QString("https://ribtoks.github.io/xpiks/donatecampaign/"); }

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
        Conectivity::SwitcherConfig m_Config;
        QString m_EngagementConfigPath;
        Helpers::LocalConfig m_EngagementConfig;
        // ----------------------------
        bool m_DonateCampaign1LinkClicked;
    };
}

#endif // SWITCHERMODEL_H
