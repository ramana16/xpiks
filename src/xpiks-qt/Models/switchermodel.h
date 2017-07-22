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
        Q_PROPERTY(bool isDonationCampaign1Active READ getIsDonationCampaign1Active NOTIFY switchesUpdated)
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
        bool getIsDonationCampaign1Active() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::DonateCampaign1); }
        bool getIsDonateCampaign1Stage2On() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::DonateCampaign1Stage2); }

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
