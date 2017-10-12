/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "uimanager.h"
#include "../Common/defines.h"
#include "../QuickBuffer/currenteditableartwork.h"
#include "../QuickBuffer/currenteditableproxyartwork.h"
#include "artworkmetadata.h"
#include "../Models/settingsmodel.h"

namespace Models {
    UIManager::UIManager(SettingsModel *settingsModel, QObject *parent) :
        QObject(parent),
        m_SettingsModel(settingsModel),
        m_TabID(42)
    {
        Q_ASSERT(settingsModel != nullptr);
        QObject::connect(m_SettingsModel, &Models::SettingsModel::keywordSizeScaleChanged, this, &UIManager::keywordHeightChanged);

        m_ActiveTabs.setSourceModel(&m_TabsModel);
        m_InactiveTabs.setSourceModel(&m_TabsModel);

        QObject::connect(&m_TabsModel, &QMLExtensions::TabsModel::tabRemoved, &m_ActiveTabs, &QMLExtensions::ActiveTabsModel::onInvalidateRequired);
        QObject::connect(&m_TabsModel, &QMLExtensions::TabsModel::cacheRebuilt, &m_ActiveTabs, &QMLExtensions::ActiveTabsModel::onInvalidateRequired);

        QObject::connect(&m_TabsModel, &QMLExtensions::TabsModel::tabRemoved, &m_InactiveTabs, &QMLExtensions::InactiveTabsModel::onInvalidateRequired);
        QObject::connect(&m_TabsModel, &QMLExtensions::TabsModel::cacheRebuilt, &m_InactiveTabs, &QMLExtensions::InactiveTabsModel::onInvalidateRequired);

        QObject::connect(&m_InactiveTabs, &QMLExtensions::InactiveTabsModel::tabOpened, &m_ActiveTabs, &QMLExtensions::ActiveTabsModel::onInactiveTabOpened);
    }

    double UIManager::getKeywordHeight() const {
        double keywordScale = m_SettingsModel->getKeywordSizeScale();
        double height = 20.0 * keywordScale + (keywordScale - 1.0) * 10.0;
        return height;
    }

    void UIManager::registerCurrentItem(std::shared_ptr<QuickBuffer::ICurrentEditable> &currentItem) {
        m_CurrentEditable = std::move(currentItem);
        emit currentEditableChanged();
    }

    void UIManager::clearCurrentItem() {
        LOG_DEBUG << "#";
        m_CurrentEditable.reset();
        emit currentEditableChanged();
    }

    QObject *UIManager::retrieveTabsModel(int tabID) {
        QObject *model = nullptr;
        if (m_TabIDsToModel.contains(tabID)) {
            model = m_TabIDsToModel[tabID];
            QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        }

        return model;
    }

    void UIManager::addSystemTab(const QString tabIconComponent, const QString &tabComponent) {
        LOG_INFO << "icon" << tabIconComponent << "contents" << tabComponent;
        m_TabsModel.addSystemTab(tabIconComponent, tabComponent);
        generateNextTabID();
    }

    int UIManager::addPluginTab(int pluginID, const QString tabIconComponent, const QString &tabComponent, QObject *tabModel) {
        LOG_INFO << "plugin" << pluginID << "icon" << tabIconComponent << "contents" << tabComponent;

        int index = m_TabsModel.rowCount();
        int id = generateNextTabID();

        m_TabsModel.addPluginTab(id, tabIconComponent, tabComponent);

        if (tabModel != nullptr) {
            Q_ASSERT(!m_TabIDsToModel.contains(id));
            m_TabIDsToModel.insert(id, tabModel);
        }

        if (!m_PluginIDToTabIDs.contains(pluginID)) {
            m_PluginIDToTabIDs.insert(pluginID, QSet<int>());
        }

        m_PluginIDToTabIDs[pluginID].insert(id);

        LOG_INFO << "Added plugin tab with ID" << id;

        return id;
    }

    bool UIManager::removePluginTab(int pluginID, int tabID) {
        LOG_INFO << "plugin" << pluginID << "tab" << tabID;
        bool success = false;

        do {
            if (!m_PluginIDToTabIDs.contains(pluginID)) {
                break;
            }

            auto &tabsSet = m_PluginIDToTabIDs[pluginID];
            if (!tabsSet.contains(tabID)) {
                break;
            }

            int index = m_TabsModel.findPluginTabIndexByID(tabID);
            if (index == -1) {
                break;
            }

            tabsSet.remove(tabID);
            m_TabsModel.removePluginTab(index);
            m_TabIDsToModel.remove(tabID);

            LOG_INFO << "Plugin's tab" << tabID << "removed";
            success = true;
        } while(false);

        if (!success) {
            LOG_WARNING << "Failed to remove plugin tab" << tabID;
        }

        return success;
    }

    void UIManager::initializeSystemTabs() {
        m_TabsModel.updateCache();

        m_TabsModel.touchTab(0);
        m_TabsModel.touchTab(1);
        m_TabsModel.touchTab(2);

        m_TabsModel.touchTab(0);
    }
}
