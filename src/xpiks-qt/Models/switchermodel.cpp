/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "switchermodel.h"
#include <QDir>
#include <QUuid>
#include "../Common/defines.h"
#include "../Helpers/stringhelper.h"

namespace Models {
#define DONATE_CAMPAIGN1_CLICKED "DonateCampaign1Clicked"
#define SWITCHER_SESSION_TOKEN "SessionToken"
#define SWITCHER_SESSION_START "SessionStart"
#define SWITCHER_TIMER_DELAY 2000

    SwitcherModel::SwitcherModel(QObject *parent):
        QObject(parent),
        Common::BaseEntity(),
        Common::StatefulEntity("switcher"),
        // effectively meaning all features are OFF
        m_Threshold(100)
    {
        QObject::connect(&m_Config, &Connectivity::SwitcherConfig::switchesUpdated,
                         this, &SwitcherModel::configUpdated);

        m_DelayTimer.setSingleShot(true);
        QObject::connect(&m_DelayTimer, &QTimer::timeout, this, &SwitcherModel::onDelayTimer);
    }

    void SwitcherModel::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);
        m_Config.setCommandManager(commandManager);
    }

    void SwitcherModel::initialize() {
        initState();
        ensureSessionTokenValid();

        QString sessionToken = getStateString(SWITCHER_SESSION_TOKEN);
        if (!sessionToken.isEmpty()) {
            quint32 hash = Helpers::switcherHash(sessionToken);
            quint32 threshold = hash % 100;
            m_Threshold = (int)threshold;
        } else {
            // effectively meaning all features are OFF
            m_Threshold = 100;
        }

        LOG_INFO << "Current threshold is" << m_Threshold;
    }

    void SwitcherModel::updateConfigs() {
        LOG_DEBUG << "#";
        m_Config.initializeConfigs();
    }

    void SwitcherModel::afterInitializedCallback() {
        LOG_DEBUG << "#";
        m_DelayTimer.start(SWITCHER_TIMER_DELAY);
    }

    void SwitcherModel::ensureSessionTokenValid() {
        LOG_DEBUG << "#";
        bool canKeepToken = false;
        const QDateTime dtNow = QDateTime::currentDateTime();

        do {
            if (!containsState(SWITCHER_SESSION_TOKEN)) {
                LOG_DEBUG << "Token not found in the state config";
                break;
            }

            const QString sessionStart = getStateString(SWITCHER_SESSION_START);
            const QDateTime sessionStartDateTime = QDateTime::fromString(sessionStart, Qt::ISODate);
            if (!sessionStartDateTime.isValid()) {
                LOG_WARNING << "Cannot parse session start datetime:" << sessionStart;
                break;
            }

            qint64 daysPassed = sessionStartDateTime.daysTo(dtNow);
            LOG_DEBUG << "Token is valid already for" << daysPassed << "day(s)";

            if ((daysPassed < 0) || (daysPassed > 30)) {
                break;
            }

            canKeepToken = true;
        } while (false);

        if (!canKeepToken) {
            LOG_INFO << "Updating switcher token";

            QUuid uuid = QUuid::createUuid();
            setStateValue(SWITCHER_SESSION_TOKEN, uuid.toString());
            QString dateTimeString = dtNow.toString(Qt::ISODate);
            setStateValue(SWITCHER_SESSION_START, dateTimeString);

            syncState();
        }
    }

    bool SwitcherModel::getDonateCampaign1LinkClicked() const {
        bool clicked = getStateBool(DONATE_CAMPAIGN1_CLICKED, false);
        return clicked;
    }

    void SwitcherModel::setDonateCampaign1LinkClicked() {
        LOG_DEBUG << "#";
        if (getDonateCampaign1LinkClicked()) { return; }

        setStateValue(DONATE_CAMPAIGN1_CLICKED, true);
        emit donateCampaign1LinkClicked();

        // save config
        syncState();
    }

    void SwitcherModel::configUpdated() {
        LOG_DEBUG << "#";
        emit switchesUpdated();
    }

    void SwitcherModel::onDelayTimer() {
        LOG_DEBUG << "#";
        LOG_DEBUG << "Donate Campaign 1 active:" << getIsDonationCampaign1On();
        LOG_DEBUG << "Donate Campaign 1 Dialog on:" << getIsDonateCampaign1Stage2On();
        LOG_DEBUG << "Donate Campaign 1 link clicked:" << getDonateCampaign1LinkClicked();

        if (getIsDonationCampaign1On() &&
                getIsDonateCampaign1Stage2On() &&
                !getDonateCampaign1LinkClicked()) {
            emit donateDialogRequested();
        }
    }
}
