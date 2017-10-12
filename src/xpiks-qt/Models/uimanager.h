/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <memory>
#include <QSet>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include "../QuickBuffer/icurrenteditable.h"
#include "../Models/metadataelement.h"
#include "../QMLExtensions/tabsmodel.h"

namespace Models {
    class ArtworkMetadata;
    class ArtworkProxyBase;
    class SettingsModel;

    class UIManager: public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool hasCurrentEditable READ getHasCurrentEditable NOTIFY currentEditableChanged)
        Q_PROPERTY(double keywordHeight READ getKeywordHeight NOTIFY keywordHeightChanged)
    public:
        explicit UIManager(Models::SettingsModel *settingsModel, QObject *parent = 0);

    private:
        int generateNextTabID() { int id = m_TabID++; return id; }

    public:
        void initTabs();
        bool getHasCurrentEditable() const { return m_CurrentEditable.operator bool(); }
        double getKeywordHeight() const;
        std::shared_ptr<QuickBuffer::ICurrentEditable> getCurrentEditable() const { return m_CurrentEditable; }

    public:
        void registerCurrentItem(std::shared_ptr<QuickBuffer::ICurrentEditable> &currentItem);
        QMLExtensions::TabsModel *getTabsModel() { return &m_TabsModel; }
        QMLExtensions::ActiveTabsModel *getActiveTabs() { return &m_ActiveTabs; }
        QMLExtensions::InactiveTabsModel *getInactiveTabs() { return &m_InactiveTabs; }

    public:
        Q_INVOKABLE void clearCurrentItem();
        Q_INVOKABLE QObject *retrieveTabsModel(int tabID);

    public:
        void addSystemTab(const QString tabIconComponent, const QString &tabComponent);
        int addPluginTab(int pluginID, const QString tabIconComponent, const QString &tabComponent, QObject *tabModel);
        bool removePluginTab(int pluginID, int tabID);
        void initializeSystemTabs();

    signals:
        void tabsListChanged();
        void tabsIconsChanged();
        void currentEditableChanged();
        void keywordHeightChanged(double value);

    private:
        Models::SettingsModel *m_SettingsModel;
        QMLExtensions::TabsModel m_TabsModel;
        QMLExtensions::ActiveTabsModel m_ActiveTabs;
        QMLExtensions::InactiveTabsModel m_InactiveTabs;
        std::shared_ptr<QuickBuffer::ICurrentEditable> m_CurrentEditable;
        QHash<int, QSet<int> > m_PluginIDToTabIDs;
        QHash<int, QObject*> m_TabIDsToModel;
        volatile int m_TabID;
    };
}

#endif // UIMANAGER_H
