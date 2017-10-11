/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "uiprovider.h"
#include <QQmlComponent>
#include <QQmlError>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQuickWindow>
#include <QHash>
#include "../Common/defines.h"

namespace Plugins {
    UIProvider::UIProvider(QObject *parent):
        QObject(parent),
        m_QmlEngine(NULL),
        m_Root(NULL)
    {
    }

    UIProvider::~UIProvider() {
    }

    void UIProvider::openDialog(const QUrl &rcPath, const QHash<QString, QObject *> &contextModels) const {
        QQmlComponent component(m_QmlEngine);

        QObject::connect(&component, &QQmlComponent::statusChanged,
                         this, &UIProvider::viewStatusChanged);

        component.loadUrl(rcPath, QQmlComponent::PreferSynchronous);

        if (!component.isReady()) {
            LOG_WARNING << "Component" << rcPath << "is not ready";
            return;
        }

        LOG_DEBUG << "Creating a new window";
        QQmlContext *context = new QQmlContext(m_QmlEngine, nullptr);
        QObject::connect(context, &QQmlContext::destroyed,
                         this, &UIProvider::contextDestroyed);

        QHashIterator<QString, QObject*> i(contextModels);
        while (i.hasNext()) {
            i.next();
            context->setContextProperty(i.key(), i.value());
        }

        QObject *object = component.create(context);

        if (object != NULL) {
            QObject::connect(object, &QObject::destroyed, this, &UIProvider::componentDestroyed);

            //object->setParent(m_Root);
            context->setParent(object);

            QQmlEngine::setObjectOwnership(object, QQmlEngine::JavaScriptOwnership);
            QQmlProperty::write(object, "parent", QVariant::fromValue<QObject*>(m_Root));

            QVariant returnedValue;
            bool result = QMetaObject::invokeMethod(object, "onAfterCreated", Q_RETURN_ARG(QVariant, returnedValue));
            if (!result) {
                LOG_WARNING << "Failed to call onAfterCreated() for" << rcPath;
            }
        } else {
            LOG_WARNING << "Failed to create object";
        }
    }

    void UIProvider::viewStatusChanged(QQmlComponent::Status status) {
        if (status == QQmlComponent::Error) {
            QQmlComponent *view = qobject_cast<QQmlComponent*>(sender());

            foreach (const QQmlError &error, view->errors()) {
                LOG_WARNING << error.description();
            }
        }
    }

    void UIProvider::componentDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "Component destroyed";
        m_QmlEngine->collectGarbage();
    }

    void UIProvider::contextDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_QmlEngine->collectGarbage();
    }
}
