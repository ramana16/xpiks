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

#include "switchermodel.h"
#include <QDir>
#include "../Common/defines.h"

namespace Models {
#ifdef QT_DEBUG
    #ifdef INTEGRATION_TESTS
        #define ENGAGEMENT_CONFIG "integration_engagement.json"
    #else
        #define ENGAGEMENT_CONFIG "debug_engagement.json"
    #endif
#else
    #define ENGAGEMENT_CONFIG "engagement.json"
#endif

#define DONATE_CAMPAIGN1_CLICKED QLatin1String("DonateCampaign1Clicked")
#define SWITCHER_TIMER_DELAY 2000

    SwitcherModel::SwitcherModel(QObject *parent):
        QObject(parent),
        Common::BaseEntity(),
        m_DonateCampaign1LinkClicked(false)
    {
        QObject::connect(&m_Config, &Conectivity::SwitcherConfig::switchesUpdated,
                         this, &SwitcherModel::configUpdated);

        m_DelayTimer.setSingleShot(true);
        QObject::connect(&m_DelayTimer, &QTimer::timeout, this, &SwitcherModel::onDelayTimer);
    }

    void SwitcherModel::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);
        m_Config.setCommandManager(commandManager);
    }

    void SwitcherModel::initEngagement() {
        LOG_DEBUG << "#";

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            m_EngagementConfigPath = appDataDir.filePath(ENGAGEMENT_CONFIG);
        } else {
            m_EngagementConfigPath = ENGAGEMENT_CONFIG;
        }

        readEngagementConfig();
    }

    void SwitcherModel::updateConfigs() {
        LOG_DEBUG << "#";
        m_Config.initializeConfigs();
    }

    void SwitcherModel::afterInitializedCallback() {
        LOG_DEBUG << "#";
        m_DelayTimer.start(SWITCHER_TIMER_DELAY);
    }

    void SwitcherModel::setDonateCampaign1LinkClicked() {
        LOG_DEBUG << "#";
        if (m_DonateCampaign1LinkClicked) { return; }

        m_DonateCampaign1LinkClicked = true;
        emit donateCampaign1LinkClicked();

        QJsonDocument &doc = m_EngagementConfig.getConfig();
        QJsonObject object;
        if (doc.isObject()) {
            object = doc.object();
        }

        object[DONATE_CAMPAIGN1_CLICKED] = m_DonateCampaign1LinkClicked;

        doc.setObject(object);

        m_EngagementConfig.saveToFile();
    }

    void SwitcherModel::configUpdated() {
        LOG_DEBUG << "#";
        emit switchesUpdated();
    }

    void SwitcherModel::onDelayTimer() {
        LOG_DEBUG << "#";
        LOG_DEBUG << "Donate Campaign 1 active:" << getIsDonationCampaign1Active();
        LOG_DEBUG << "Donate Campaign 1 Dialog on:" << getIsDonateCampaign1Stage2On();
        LOG_DEBUG << "Donate Campaign 1 link clicked:" << getDonateCampaign1LinkClicked();

        if (getIsDonationCampaign1Active() &&
                getIsDonateCampaign1Stage2On() &&
                !getDonateCampaign1LinkClicked()) {
            emit donateDialogRequested();
        }
    }

    void SwitcherModel::readEngagementConfig() {
        LOG_DEBUG << "#";

        m_EngagementConfig.initConfig(m_EngagementConfigPath);

        const QJsonDocument &doc = m_EngagementConfig.getConfig();
        if (!doc.isObject()) { return; }

        QJsonObject object = doc.object();
        QJsonValue campaign1Clicked = object[DONATE_CAMPAIGN1_CLICKED];
        if (campaign1Clicked.isBool()) {
            m_DonateCampaign1LinkClicked = campaign1Clicked.toBool();
        }

        LOG_DEBUG << "Donate Campaign #1 link clicked =" << m_DonateCampaign1LinkClicked;
    }
}
