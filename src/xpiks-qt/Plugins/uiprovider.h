/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UIPROVIDER_H
#define UIPROVIDER_H

#include <QQuickItem>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QObject>
#include <QUrl>

namespace Models {
    class UIManager;
}

namespace Plugins {
    class UIProvider : public QObject
    {
        Q_OBJECT
    public:
        UIProvider(QObject *parent=0);
        virtual ~UIProvider();

    public:
        Models::UIManager *getUIManager() const { return m_UiManager; }

    public:
        void setQmlEngine(QQmlEngine *engine) { m_QmlEngine = engine; }
        void setRoot(QQuickItem *root) { m_Root = root; }
        void setUIManager(Models::UIManager *manager) { m_UiManager = manager; }

        // IUIProvider interface
    public:
        void openDialog(const QUrl &rcPath, const QHash<QString, QObject*> &contextModels = QHash<QString, QObject*>()) const;

    private slots:
        void viewStatusChanged(QQmlComponent::Status status);
        void componentDestroyed(QObject *object);
        void contextDestroyed(QObject *object);

    private:
        QQmlEngine *m_QmlEngine;
        QQuickItem *m_Root;
        Models::UIManager *m_UiManager;
    };
}

#endif // UIPROVIDER_H
