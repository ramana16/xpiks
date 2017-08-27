/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "switcherconfig.h"
#include <QDir>
#include <QJsonValue>
#include "apimanager.h"
#include "../Common/defines.h"

namespace Connectivity {
#if defined(QT_DEBUG)
    #define LOCAL_SWITCHER_CONFIG "debug_switches.json"
#elif defined(INTEGRATION_TESTS)
#define LOCAL_SWITCHER_CONFIG "tests_switches.json"
#else
    #define LOCAL_SWITCHER_CONFIG "switches.json"
#endif

#define VALUE_KEY QLatin1String("v")
#define THRESHOLD_KEY QLatin1String("t")
#define WIN_THRESHOLD_KEY QLatin1String("wt")
#define MAC_THRESHOLD_KEY QLatin1String("mt")
#define LIN_THRESHOLD_KEY QLatin1String("lt")

#define OVERWRITE_KEY QLatin1String("overwrite")
#define OVERWRITE_SWITCHER_CONFIG true
#define DONATE_CAMPAIGN_1_KEY QLatin1String("donateCampaign1")
#define DONATE_CAMPAIGN_1_STAGE_2 QLatin1String("donateCampaign1Stage2")
#define PROGRESSIVE_SUGGESTION_PREVIEWS QLatin1String("progressiveSuggestionPreviews")
#define DIRECT_METADATA_EXPORT QLatin1String("directExport")
#define GOOD_QUALITY_VIDEO_PREVIEWS QLatin1String("qualityVideoPreviews")

    QDebug operator << (QDebug d, const SwitcherConfig::SwitchValue &value) {
        d << "{" << value.m_IsOn << "*" << value.m_Threshold << "}";
        return d;
    }

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

        auto &apiManager = Connectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getSwitcherAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);
    }

    bool SwitcherConfig::isSwitchOn(int switchKey, int minThreshold) {
        QReadLocker locker(&m_RwLock);
        Q_UNUSED(locker);
        bool isOn = false;

        if (m_SwitchesHash.contains(switchKey)) {
            SwitchValue &value = m_SwitchesHash[switchKey];
            if (value.m_Threshold >= minThreshold) {
                isOn = value.m_IsOn;
            }
        }

        return isOn;
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

    bool tryGetDouble(const QJsonObject &object, const QString &key, double &value) {
        bool hasValue = false;

        if (object.contains(key)) {
            QJsonValue valueObject = object[key];
            if (valueObject.isDouble()) {
                value = valueObject.toDouble();
                hasValue = true;
            }
        }

        return hasValue;
    }

    bool tryGetThresholdValue(const QJsonObject &object, double &threshold) {
#if defined(Q_OS_WIN)
        if (tryGetDouble(object, WIN_THRESHOLD_KEY, threshold)) {
            return true;
        }
#elif defined(Q_OS_MAC)
        if (tryGetDouble(object, MAC_THRESHOLD_KEY, threshold)) {
            return true;
        }
#elif defined(Q_OS_LINUX)
        if (tryGetDouble(object, LIN_THRESHOLD_KEY, threshold)) {
            return true;
        }
#endif

        if (tryGetDouble(object, THRESHOLD_KEY, threshold)) {
            return true;
        }

        return false;
    }

    bool tryGetBool(const QJsonObject &object, const QString &key, bool &value) {
        bool hasValue = false;

        if (object.contains(key)) {
            QJsonValue valueObject = object[key];
            if (valueObject.isBool()) {
                value = valueObject.toBool();
                hasValue = true;
            }
        }

        return hasValue;
    }

    void initSwitchValue(const QJsonObject &object, const QLatin1String &keyName, SwitcherConfig::SwitchValue &value) {
        value.m_IsOn = false;
        value.m_Threshold = 0;

        QJsonValue propertiesObjectValue = object[keyName];
        if (propertiesObjectValue.isObject()) {
            QJsonObject propertiesObject = propertiesObjectValue.toObject();

            bool switchValue = false;
            if (tryGetBool(propertiesObject, VALUE_KEY, switchValue)) {
                value.m_IsOn = switchValue;
            }

            double threshold = 0.0;
            if (tryGetThresholdValue(propertiesObject, threshold)) {
                int intThreshold = (int)threshold;
                Q_ASSERT((0 <= intThreshold) && (intThreshold <= 100));
                if ((0 <= intThreshold) && (intThreshold <= 100)) {
                    value.m_Threshold = intThreshold;
                }
            }
        }
    }

    void SwitcherConfig::parseSwitches(const QJsonObject &object) {
        LOG_DEBUG << "#";

        SwitchValue donateCampaign1Active;
        SwitchValue donateCampaign1Stage2;
        SwitchValue progressiveSuggestionPreviews;
        SwitchValue directExport;
        SwitchValue qualityVideoPreviews;

        initSwitchValue(object, DONATE_CAMPAIGN_1_KEY, donateCampaign1Active);
        initSwitchValue(object, DONATE_CAMPAIGN_1_STAGE_2, donateCampaign1Stage2);
        initSwitchValue(object, PROGRESSIVE_SUGGESTION_PREVIEWS, progressiveSuggestionPreviews);
        initSwitchValue(object, DIRECT_METADATA_EXPORT, directExport);
        initSwitchValue(object, GOOD_QUALITY_VIDEO_PREVIEWS, qualityVideoPreviews);

        // overwrite these values
        {
            QWriteLocker locker(&m_RwLock);
            Q_UNUSED(locker);

            m_SwitchesHash.clear();
            m_SwitchesHash[DonateCampaign1] = donateCampaign1Active;
            m_SwitchesHash[DonateCampaign1Stage2] = donateCampaign1Stage2;
            m_SwitchesHash[ProgressiveSuggestionPreviews] = progressiveSuggestionPreviews;
            m_SwitchesHash[DirectMetadataExport] = directExport;
            m_SwitchesHash[GoodQualityVideoPreviews] = qualityVideoPreviews;

            LOG_INTEGR_TESTS_OR_DEBUG << m_SwitchesHash;
        }

        emit switchesUpdated();
    }
}
