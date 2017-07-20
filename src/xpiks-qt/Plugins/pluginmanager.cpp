/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pluginmanager.h"
#include <QPluginLoader>
#include <QApplication>
#include <QDir>
#include <QQmlEngine>
#include "xpiksplugininterface.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "pluginwrapper.h"
#include "../Models/artitemsmodel.h"
#include "../Common/defines.h"
#include "../Helpers/constants.h"

namespace Plugins {
    PluginManager::PluginManager():
        QAbstractListModel(),
        m_LastPluginID(0)
    {
    }

    PluginManager::~PluginManager() {
        LOG_DEBUG << "#";
    }

    bool PluginManager::getPluginsDir(QDir &pluginsDir) {
        LOG_DEBUG << "#";
        QString appDataPath = XPIKS_USERDATA_PATH;
        bool pluginsFound = false;

        if (!appDataPath.isEmpty()) {
            QString pluginsPath = QDir::cleanPath(appDataPath + QDir::separator() + Constants::PLUGINS_DIR);
            if (QFileInfo(pluginsPath).exists()) {
                pluginsDir.setPath(pluginsPath);
                pluginsFound = true;
            }
        }

        if (!pluginsFound) {
            pluginsDir.setPath(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)
            if (pluginsDir.dirName() == "MacOS") {
                pluginsDir.cdUp();
            }

            pluginsDir.cd("PlugIns");
#endif

            pluginsFound = pluginsDir.cd("XpiksPlugins");

            if (!pluginsFound) {
                LOG_WARNING << "Plugins directory not found";
            }
        }

        return pluginsFound;
    }

    void PluginManager::loadPlugins() {
        LOG_DEBUG << "#";
        QDir pluginsDir;

        if (!getPluginsDir(pluginsDir)) {
            return;
        }

        LOG_INFO << "Plugins dir:" << pluginsDir.absolutePath();
        std::vector<std::shared_ptr<PluginWrapper> > loadedPlugins;
        QHash<int, std::shared_ptr<PluginWrapper> > pluginsDict;

        QStringList pluginDirFiles = pluginsDir.entryList(QDir::Files);
        LOG_INFO << "Trying to load" << pluginDirFiles.size() << "file(s)";

        foreach (QString fileName, pluginDirFiles) {
            LOG_DEBUG << "Trying file:" << fileName;

            try {
                QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
                QObject *plugin = loader.instance();
                if (plugin) {
                    XpiksPluginInterface *xpiksPlugin = qobject_cast<XpiksPluginInterface *>(plugin);

                    if (xpiksPlugin) {
                        auto pluginWrapper = instantiatePlugin(xpiksPlugin);
                        if (pluginWrapper) {
                            loadedPlugins.push_back(pluginWrapper);
                            pluginsDict.insert(pluginWrapper->getPluginID(), pluginWrapper);
                        }
                    } else {
                        LOG_DEBUG << "Not Xpiks Plugin:" << fileName;
                    }
                } else {
                    LOG_WARNING << loader.errorString();
                }
            } catch(...) {
                LOG_WARNING << "Exception while loading" << fileName;
            }
        }

        if (!loadedPlugins.empty()) {
            LOG_DEBUG << "Resetting plugins model";
            beginResetModel();
            {
                m_PluginsList.swap(loadedPlugins);
                m_PluginsDict.swap(pluginsDict);

                loadedPlugins.clear();
                pluginsDict.clear();
            }
            endResetModel();
        }
    }

    void PluginManager::unloadPlugins() {
        size_t size = m_PluginsList.size();
        LOG_DEBUG << size << "plugin(s)";

        for (size_t i = 0; i < size; ++i) {
            auto &wrapper = m_PluginsList.at(i);
            wrapper->finalizePlugin();
        }
    }

    bool PluginManager::hasExportedActions(int row) const {
        bool hasActions = false;

        if ((0 <= row) && (row < rowCount())) {
            hasActions = m_PluginsList.at(row)->anyActionsProvided();
        }

        return hasActions;
    }

    void PluginManager::onCurrentEditableChanged() {
        LOG_DEBUG << "#";
        size_t size = m_PluginsList.size();
        QVariant empty;

        for (size_t i = 0; i < size; ++i) {
            std::shared_ptr<PluginWrapper> &wrapper = m_PluginsList.at(i);
            wrapper->notifyPlugin(Plugins::PluginNotificationFlags::CurrentEditableChanged, empty, nullptr);
        }
    }

    void PluginManager::onLastActionUndone(int commandID) {
        LOG_DEBUG << "#";
        size_t size = m_PluginsList.size();
        QVariant commandVariant = QVariant::fromValue(commandID);

        for (size_t i = 0; i < size; ++i) {
            std::shared_ptr<PluginWrapper> &wrapper = m_PluginsList.at(i);
            wrapper->notifyPlugin(Plugins::PluginNotificationFlags::ActionUndone, commandVariant, nullptr);
        }
    }

    void PluginManager::onPresetsUpdated() {
        LOG_DEBUG << "#";
        size_t size = m_PluginsList.size();
        QVariant empty;

        for (size_t i = 0; i < size; ++i) {
            std::shared_ptr<PluginWrapper> &wrapper = m_PluginsList.at(i);
            wrapper->notifyPlugin(Plugins::PluginNotificationFlags::PresetsUpdated, empty, nullptr);
        }
    }

    QObject *PluginManager::getPluginActions(int index) const {
        LOG_DEBUG << index;
        PluginActionsModel *item = NULL;

        if ((0 <= index) && (index < rowCount())) {
            item = m_PluginsList.at(index)->getActionsModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    void PluginManager::triggerPluginAction(int pluginID, int actionID) const {
        LOG_INFO << "Plugin ID" << pluginID << "action ID" << actionID;
        std::shared_ptr<PluginWrapper> pluginWrapper = m_PluginsDict.value(pluginID, std::shared_ptr<PluginWrapper>());
        if (pluginWrapper) {
            pluginWrapper->triggerActionSafe(actionID);
        }
    }

    std::shared_ptr<PluginWrapper> PluginManager::instantiatePlugin(XpiksPluginInterface *plugin) {
        int pluginID = getNextPluginID();
        LOG_INFO << "ID:" << pluginID << "name:" << plugin->getPrettyName() << "version:" << plugin->getVersionString();

        std::shared_ptr<PluginWrapper> pluginWrapper(new PluginWrapper(plugin, pluginID, &m_UIProvider));

        try {
            plugin->injectCommandManager(m_CommandManager);
            plugin->injectUndoRedoManager(m_CommandManager->getUndoRedoManager());
            plugin->injectUIProvider(pluginWrapper->getUIProvider());
            plugin->injectArtworksSource(m_CommandManager->getArtItemsModel());
            plugin->injectPresetsManager(m_CommandManager->getPresetsModel());

            plugin->initializePlugin();
            // TODO: check this in config in future
            plugin->enablePlugin();
        }
        catch(...) {
            LOG_WARNING << "Fail initializing plugin with ID:" << pluginID;
            pluginWrapper.reset();
        }

        return pluginWrapper;
    }

    int PluginManager::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_PluginsList.size();
    }

    QVariant PluginManager::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= rowCount()) { return QVariant(); }

        auto &plugin = m_PluginsList.at(row);

        switch (role) {
        case PrettyNameRole:
            return plugin->getPrettyName();
        case AuthorRole:
            return plugin->getAuthor();
        case VersionRole:
            return plugin->getVersionString();
        case PluginIDRole:
            return plugin->getPluginID();
        case IsEnabledRole:
            return plugin->getIsEnabled();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> PluginManager::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PrettyNameRole] = "prettyname";
        roles[AuthorRole] = "author";
        roles[VersionRole] = "version";
        roles[PluginIDRole] = "pluginID";
        roles[IsEnabledRole] = "enabled";
        return roles;
    }

    int PluginsWithActionsModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();

        return row;
    }

    bool PluginsWithActionsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        QAbstractItemModel *sourceItemModel = sourceModel();
        PluginManager *pluginManager = dynamic_cast<PluginManager *>(sourceItemModel);
        Q_ASSERT(pluginManager != NULL);
        bool result = pluginManager->hasExportedActions(sourceRow);
        return result;
    }

}
