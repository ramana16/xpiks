/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SANDBOXEDDEPENDENCIES_H
#define SANDBOXEDDEPENDENCIES_H

#include "../Plugins/iuiprovider.h"

namespace Models {
    class UIManager;
}

namespace Plugins {
    class UIProvider;

    class UIProviderSafe : public IUIProvider {
    public:
        UIProviderSafe(int pluginID, UIProvider *realUIProvider);

    public:
        virtual void openWindow(const QUrl &rcPath, const QHash<QString, QObject*> &contextModels = QHash<QString, QObject*>()) const override;
        virtual int addTab(const QString &tabIconUrl, const QString &tabComponentUrl, QObject *tabModel) const override;
        virtual bool removeTab(int tabID) const override;
        virtual std::shared_ptr<QuickBuffer::ICurrentEditable> getCurrentEditable() const override;

    private:
        int m_PluginID;
        UIProvider *m_RealUIProvider;
    };
}

#endif // SANDBOXEDDEPENDENCIES_H
