/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sandboxeddependencies.h"
#include "../Models/uimanager.h"
#include "uiprovider.h"

namespace Plugins {
    UIProviderSafe::UIProviderSafe(int pluginID, UIProvider *realUIProvider):
        m_PluginID(pluginID),
        m_RealUIProvider(realUIProvider)
    {
        Q_ASSERT(realUIProvider != nullptr);
    }

    void UIProviderSafe::openWindow(const QUrl &rcPath, const QHash<QString, QObject *> &contextModels) const {
        m_RealUIProvider->openWindow(rcPath, contextModels);
    }

    int UIProviderSafe::addTab(const QString &tabIconUrl, const QString &tabComponentUrl, QObject *tabModel) const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);
        int result = uiManager->addPluginTab(m_PluginID, tabIconUrl, tabComponentUrl, tabModel);
        return result;
    }

    bool UIProviderSafe::removeTab(int tabID) const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);
        bool result = uiManager->removePluginTab(m_PluginID, tabID);
        return result;
    }

    std::shared_ptr<QuickBuffer::ICurrentEditable> UIProviderSafe::getCurrentEditable() const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);

        return uiManager->getCurrentEditable();
    }
}
