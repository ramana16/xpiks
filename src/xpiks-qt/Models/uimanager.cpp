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

#define MAX_SAVE_PAUSE_RESTARTS 5

#define DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH 300

#define DEFAULT_APP_WIDTH 900
#define DEFAULT_APP_HEIGHT 725
#define DEFAULT_APP_POSITION -1

namespace Models {
    UIManager::UIManager(SettingsModel *settingsModel, QObject *parent) :
        QObject(parent),
        Common::StatefulEntity("uimanager"),
        Common::DelayedActionEntity(500, MAX_SAVE_PAUSE_RESTARTS),
        m_SettingsModel(settingsModel),
        m_TabID(42),
        m_SaveTimerId(-1),
        m_SaveRestartsCount(0)
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
        LOG_DEBUG << "#";
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

    int UIManager::getArtworkEditRightPaneWidth() {
        return getStateInt(Constants::artworkEditRightPaneWidth, DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH);
    }

    void UIManager::setArtworkEditRightPaneWidth(int value) {
        const int current = getArtworkEditRightPaneWidth();
        if (current != value) {
            setStateValue(Constants::artworkEditRightPaneWidth, value);
            justChanged();
            emit artworkEditRightPaneWidthChanged();
        }
    }

    int UIManager::getAppWidth(int defaultWidth)  {
        return getStateInt(Constants::appWindowWidth, defaultWidth);
    }

    void UIManager::setAppWidth(int width) {
        LOG_DEBUG << width;
        setStateValue(Constants::appWindowWidth, width);
        justChanged();
    }

    int UIManager::getAppHeight(int defaultHeight) {
        return getStateInt(Constants::appWindowHeight, defaultHeight);
    }

    void UIManager::setAppHeight(int height) {
        LOG_DEBUG << height;
        setStateValue(Constants::appWindowHeight, height);
        justChanged();
    }

    int UIManager::getAppPosX(int defaultPosX) {
        int posX = getStateInt(Constants::appWindowX, defaultPosX);
        if (posX == -1) { posX = defaultPosX; }
        return posX;
    }

    void UIManager::setAppPosX(int x) {
        LOG_DEBUG << x;
        setStateValue(Constants::appWindowX, x);
        justChanged();
    }

    int UIManager::getAppPosY(int defaultPosY) {
        int posY = getStateInt(Constants::appWindowY, defaultPosY);
        if (posY == -1) { posY = defaultPosY; }
        return posY;
    }

    void UIManager::setAppPosY(int y) {
        LOG_DEBUG << y;
        setStateValue(Constants::appWindowY, y);
        justChanged();
    }

    void UIManager::activateQuickBufferTab() {
        if (m_TabsModel.activateSystemTab(QUICKBUFFER_TAB_ID)) {
            m_ActiveTabs.reactivateMostRecentTab();
        }
    }

    void UIManager::addSystemTab(int systemTabID, const QString tabIconComponent, const QString &tabComponent) {
        LOG_INFO << "icon" << tabIconComponent << "contents" << tabComponent;
        m_TabsModel.addSystemTab(systemTabID, tabIconComponent, tabComponent);
        generateNextTabID();
    }

    int UIManager::addPluginTab(int pluginID, const QString tabIconComponent, const QString &tabComponent, QObject *tabModel) {
        LOG_INFO << "plugin" << pluginID << "icon" << tabIconComponent << "contents" << tabComponent;

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

    void UIManager::initializeState() {
         initState();
    }

    void UIManager::resetWindowSettings() {
        // resetting position in settings is pretty useless because
        // we will overwrite them on Xpiks exit. But anyway for the future...
        setStateValue(Constants::appWindowHeight, DEFAULT_APP_HEIGHT);
        setStateValue(Constants::appWindowWidth, DEFAULT_APP_WIDTH);
        setStateValue(Constants::appWindowX, DEFAULT_APP_POSITION);
        setStateValue(Constants::appWindowY, DEFAULT_APP_POSITION);

        setStateValue(Constants::artworkEditRightPaneWidth, DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH);

        justChanged();
    }

    void UIManager::doOnTimer() {
        syncState();
    }
}
