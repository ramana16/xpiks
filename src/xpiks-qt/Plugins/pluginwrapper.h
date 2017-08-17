/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PLUGINWRAPPER_H
#define PLUGINWRAPPER_H

#include <QObject>
#include <QString>
#include <cstdint>
#include "pluginactionsmodel.h"
#include "sandboxeddependencies.h"
#include "iuiprovider.h"
#include "xpiksplugininterface.h"
#include "../Common/flags.h"

namespace Plugins {
    class UIProvider;

    class PluginWrapper
    {
    public:
        PluginWrapper(const QString &filepath, XpiksPluginInterface *pluginInterface, int pluginID, UIProvider *realUIProvider);
        virtual ~PluginWrapper();

    public:
        int getPluginID() const { return m_PluginID; }
        bool getIsEnabled() const { return m_IsEnabled && !m_IsRemoved; }
        bool getIsRemoved() const { return m_IsRemoved; }
        const QString &getFilepath() const { return m_PluginFilepath; }
        const QString &getPrettyName() const { return m_PrettyName; }
        const QString &getVersionString() const { return m_VersionString; }
        const QString &getAuthor() const { return m_Author; }

        bool anyActionsProvided() const { return m_ActionsModel.size() > 0; }
        PluginActionsModel *getActionsModel() { return &m_ActionsModel; }
        IUIProvider *getUIProvider() { return &m_UIProviderSafe; }

        void enablePlugin();
        void disablePlugin();

        void triggerActionSafe(int actionID) const;
        void finalizePlugin();
        void removePlugin() { m_IsRemoved = true; }

        void notifyPlugin(PluginNotificationFlags flag, const QVariant &data, void *pointer);

    private:
        XpiksPluginInterface *m_PluginInterface;
        PluginActionsModel m_ActionsModel;
        Common::flag_t m_NotificationFlags;
        UIProviderSafe m_UIProviderSafe;
        QString m_PluginFilepath;
        int m_PluginID;
        bool m_IsEnabled;
        bool m_IsRemoved;
        const QString &m_PrettyName;
        const QString &m_VersionString;
        const QString &m_Author;
    };
}

#endif // PLUGINWRAPPER_H
