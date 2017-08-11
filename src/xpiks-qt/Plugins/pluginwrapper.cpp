/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "pluginwrapper.h"
#include "xpiksplugininterface.h"
#include "ipluginaction.h"
#include "pluginactionsmodel.h"
#include "uiprovider.h"
#include "../Common/defines.h"

namespace Plugins {
    PluginWrapper::PluginWrapper(const QString &filepath, XpiksPluginInterface *pluginInterface, int pluginID, UIProvider *realUIProvider):
        m_PluginInterface(pluginInterface),
        m_ActionsModel(pluginInterface->getExportedActions(), pluginID),
        m_NotificationFlags(pluginInterface->getDesiredNotificationFlags()),
        m_UIProviderSafe(pluginID, realUIProvider),
        m_PluginFilepath(filepath),
        m_PluginID(pluginID),
        m_IsEnabled(true),
        m_PrettyName(pluginInterface->getPrettyName()),
        m_VersionString(pluginInterface->getVersionString()),
        m_Author(pluginInterface->getAuthor())
    {
        Q_ASSERT(realUIProvider != nullptr);
    }

    PluginWrapper::~PluginWrapper() {
        LOG_DEBUG << m_PluginID;
    }

    void PluginWrapper::enablePlugin() {
        LOG_INFO << getPrettyName() << getVersionString();

        try {
            m_PluginInterface->enablePlugin();
            m_IsEnabled = true;
        }
        catch(...) {
            LOG_WARNING << "Exception while enabling plugin";
        }
    }

    void PluginWrapper::disablePlugin() {
        LOG_INFO << getPrettyName() << getVersionString();

        try {
            // set disabled in any case
            m_IsEnabled = false;
            m_PluginInterface->disablePlugin();
        }
        catch(...) {
            LOG_WARNING << "Exception while disabling plugin";
        }
    }

    void PluginWrapper::triggerActionSafe(int actionID) const {
        LOG_INFO << getPrettyName() << "executing action:" << actionID;

        try {
            if (m_IsEnabled) {
                m_PluginInterface->executeAction(actionID);
            } else {
                LOG_WARNING << getPrettyName() << "is disabled";
            }
        }
        catch (...) {
            LOG_WARNING << "Exception while triggering action for plugin ID" << m_PluginID;
        }
    }

    void PluginWrapper::finalizePlugin() {
        LOG_INFO << getPrettyName() << getVersionString();

        try {
            m_PluginInterface->finalizePlugin();
        }
        catch (...) {
            LOG_WARNING << "Exception on finalization";
        }
    }

    void PluginWrapper::notifyPlugin(PluginNotificationFlags flag, const QVariant &data, void *pointer) {
        if (m_IsEnabled) {
            if (Common::HasFlag(m_NotificationFlags, flag)) {
                m_PluginInterface->onPropertyChanged(flag, data, pointer);
            } else {
                LOG_DEBUG << "Plugin" << m_PrettyName << "is not subscribed to" << (Common::flag_t)flag;
            }
        } else {
            LOG_DEBUG << "Plugin" << m_PrettyName << "is disabled";
        }
    }
}

