/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <vector>
#include <memory>
#include <QHash>
#include <QDir>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "../Common/baseentity.h"
#include "uiprovider.h"

namespace Plugins {
    class XpiksPluginInterface;
    class PluginWrapper;

    class PluginManager : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        PluginManager();
        virtual ~PluginManager();

    public:
        enum UploadInfoRepository_Roles {
            PrettyNameRole = Qt::UserRole + 1,
            VersionRole,
            AuthorRole,
            PluginIDRole,
            IsEnabledRole
        };

    private:
        bool initPluginsDir();
        const QString &getPluginsDirectoryPath() const { return m_PluginsDirectoryPath; }
        void processInvalidFile(const QString &filename, const QString &pluginFullPath);

    public:
        void loadPlugins();
        void unloadPlugins();
        bool hasExportedActions(int row) const;
        UIProvider *getUIProvider() { return &m_UIProvider; }

    public slots:
        void onCurrentEditableChanged();
        void onLastActionUndone(int commandID);
        void onPresetsUpdated();

    public:
        Q_INVOKABLE QObject *getPluginActions(int index) const;
        Q_INVOKABLE void triggerPluginAction(int pluginID, int actionID) const;
        Q_INVOKABLE bool removePlugin(int index);
        Q_INVOKABLE bool pluginExists(const QUrl &pluginUrl);
        Q_INVOKABLE bool installPlugin(const QUrl &pluginUrl);
        Q_INVOKABLE bool replaceInstallPlugin(const QUrl &pluginUrl);

    private:
        bool addPlugin(const QString &fullpath);
        bool doAddPlugin(const QString &filepath);
        int findPluginIndex(const QString &fullpath) const;
        bool isPluginAdded(const QString &fullpath) const { return findPluginIndex(fullpath) != -1; }
        std::shared_ptr<PluginWrapper> loadPlugin(const QString &filepath);
        std::shared_ptr<PluginWrapper> instantiatePlugin(const QString &filepath, XpiksPluginInterface *plugin);

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        int getNextPluginID() { return m_LastPluginID++; }

    private:
        QString m_PluginsDirectoryPath;
        QString m_FailedPluginsDirectory;
        std::vector<std::shared_ptr<PluginWrapper> > m_PluginsList;
        QHash<int, std::shared_ptr<PluginWrapper> > m_PluginsDict;
        UIProvider m_UIProvider;
        int m_LastPluginID;
    };

    class PluginsWithActionsModel: public QSortFilterProxyModel {
        Q_OBJECT
    public:
        PluginsWithActionsModel(QObject *parent = 0):
            QSortFilterProxyModel(parent)
        {}

    public:
        Q_INVOKABLE int getOriginalIndex(int index);

    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    };
}

#endif // PLUGINMANAGER_H
