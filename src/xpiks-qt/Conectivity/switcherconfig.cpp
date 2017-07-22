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

#include "switcherconfig.h"
#include <QDir>
#include <QJsonValue>
#include "apimanager.h"

namespace Conectivity {
#ifdef QT_DEBUG
    #define LOCAL_SWITCHER_CONFIG "debug_switches.json"
#else
    #define LOCAL_SWITCHER_CONFIG "switches.json"
#endif

#define OVERWRITE_KEY QLatin1String("overwrite")
#define OVERWRITE_SWITCHER_CONFIG true
#define DONATE_CAMPAIGN_1_KEY QLatin1String("donateCampaign1")
#define DONATE_CAMPAIGN_1_STAGE_2 QLatin1String("donateCampaign1Stage2")

    SwitcherConfig::SwitcherConfig(QObject *parent):
        Models::AbstractConfigUpdaterModel(OVERWRITE_SWITCHER_CONFIG, parent)
    {
    }

    void SwitcherConfig::initializeConfigs() {
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_SWITCHER_CONFIG);
        } else {
            localConfigPath = LOCAL_SWITCHER_CONFIG;
        }

        auto &apiManager = Conectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getSwitcherAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);
    }

    bool SwitcherConfig::isSwitchOn(int switchKey) {
        QReadLocker locker(&m_RwLock);
        Q_UNUSED(locker);
        bool result = m_SwitchesHash.value(switchKey, false);
        return result;
    }

    bool SwitcherConfig::processLocalConfig(const QJsonDocument &document) {
        LOG_INTEGR_TESTS_OR_DEBUG << document;
        bool anyError = false;

        do {
            if (!document.isObject()) {
                LOG_WARNING << "Json document is not an object";
                anyError = true;
                break;
            }

            QJsonObject rootObject = document.object();
            parseSwitches(rootObject);
        } while (false);

        return anyError;
    }

    void SwitcherConfig::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        bool overwrite = false;

        if (!overwriteLocal && remoteDocument.isObject()) {
            QJsonObject rootObject = remoteDocument.object();
            if (rootObject.contains(OVERWRITE_KEY)) {
                QJsonValue overwriteValue = rootObject[OVERWRITE_KEY];
                if (overwriteValue.isBool()) {
                    overwrite = overwriteValue.toBool();
                    LOG_DEBUG << "Overwrite flag present in the config:" << overwrite;
                } else {
                    LOG_WARNING << "Overwrite flag is not boolean";
                }
            }
        } else {
            overwrite = overwriteLocal;
        }

        Models::AbstractConfigUpdaterModel::processRemoteConfig(remoteDocument, overwrite);

        if (remoteDocument.isObject()) {
            parseSwitches(remoteDocument.object());
        } else {
            LOG_WARNING << "Remote document is not an object";
        }
    }

    int SwitcherConfig::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        // values are always considered equal. This may lead to loss of local changes.
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
    }

    bool getSwitchValue(const QJsonObject &object, const QLatin1String &keyName) {
        bool isOn = false;

        QJsonValue isOnValue = object[keyName];
        if (isOnValue.isBool()) {
            isOn = isOnValue.toBool();
        }

        return isOn;
    }

    void SwitcherConfig::parseSwitches(const QJsonObject &object) {
        LOG_DEBUG << "#";

        bool donateCampaign1Active = getSwitchValue(object, DONATE_CAMPAIGN_1_KEY);
        bool donateCampaign1Stage2 = getSwitchValue(object, DONATE_CAMPAIGN_1_STAGE_2);

        // overwrite these values
        {
            QWriteLocker locker(&m_RwLock);
            m_SwitchesHash.clear();
            m_SwitchesHash[DonateCampaign1] = donateCampaign1Active;
            m_SwitchesHash[DonateCampaign1Stage2] = donateCampaign1Stage2;
        }

        emit switchesUpdated();
    }
}
