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

    bool PluginManager::retrievePluginsDir(QDir &pluginsDir) {
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

        if (pluginsFound) {
            m_PluginsDirectoryPath = pluginsDir.absolutePath();
            LOG_INFO << "Plugins directory:" << m_PluginsDirectoryPath;
        }

        return pluginsFound;
    }

    void PluginManager::loadPlugins() {
        LOG_DEBUG << "#";
        QDir pluginsDir;

        if (!retrievePluginsDir(pluginsDir)) {
            return;
        }

        LOG_INFO << "Plugins dir:" << pluginsDir.absolutePath();
        std::vector<std::shared_ptr<PluginWrapper> > loadedPlugins;
        QHash<int, std::shared_ptr<PluginWrapper> > pluginsDict;

        QStringList pluginDirFiles = pluginsDir.entryList(QDir::Files);
        LOG_INFO << "Trying to load" << pluginDirFiles.size() << "file(s)";

        foreach (QString fileName, pluginDirFiles) {
            LOG_DEBUG << "Trying file:" << fileName;

            const QString pluginFullPath = pluginsDir.absoluteFilePath(fileName);
            auto pluginWrapper = loadPlugin(pluginFullPath);
            if (pluginWrapper) {
                loadedPlugins.push_back(pluginWrapper);
                pluginsDict.insert(pluginWrapper->getPluginID(), pluginWrapper);
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
            wrapper->disablePlugin();
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

    void PluginManager::removePlugin(int index) {
        LOG_INFO << "index:" << index;

        if ((0 <= index) && (index < rowCount())) {
            std::shared_ptr<PluginWrapper> wrapper = m_PluginsList.at(index);
            const int pluginID = wrapper->getPluginID();
            LOG_INFO << "Removing plugin with ID:" << pluginID;

            wrapper->disablePlugin();
            wrapper->finalizePlugin();

            beginRemoveRows(QModelIndex(), index, index);
            {
                m_PluginsList.erase(m_PluginsList.begin() + index);
            }
            endRemoveRows();

            int removedCount = m_PluginsDict.remove(pluginID);
            Q_ASSERT(removedCount == 1);

            const QString fullPath = wrapper->getFilepath();
            if (!QFile::remove(fullPath)) {
                LOG_WARNING << "Failed to remove file" << fullPath;
            }

            LOG_INFO << "Plugin with ID #" << pluginID << "removed";
        }
    }

    bool PluginManager::pluginExists(const QUrl &pluginUrl) {
        bool exists = false;
        QString fullpath = pluginUrl.toLocalFile();

        QFileInfo existingFI(fullpath);
        if (existingFI.exists()) {
            const QString filename = existingFI.fileName();
            QString destinationPath = QDir::cleanPath(m_PluginsDirectoryPath + QDir::separator() + filename);
            exists = QFileInfo(destinationPath).exists();
        }

        return exists;
    }

    bool PluginManager::installPlugin(const QUrl &pluginUrl) {
        bool result = addPlugin(pluginUrl.toLocalFile());
        return result;
    }

    bool PluginManager::addPlugin(const QString &fullpath) {
        LOG_INFO << fullpath;
        bool success = false;

        do {
            QFileInfo existingFI(fullpath);
            if (!existingFI.exists()) {
                LOG_WARNING << "Path not found:" << fullpath;
                break;
            }

            const QString filename = existingFI.fileName();
            QString destinationPath = QDir::cleanPath(m_PluginsDirectoryPath + QDir::separator() + filename);
            if (QFileInfo(destinationPath).exists()) {
                LOG_WARNING << "Plugin already exists";
                break;
            }

            if (!QFile::copy(fullpath, destinationPath)) {
                LOG_WARNING << "Failed to copy plugin to" << destinationPath;
                break;
            }

            if (!doAddPlugin(destinationPath)) {
                if (!QFile::remove(destinationPath)) {
                    LOG_WARNING << "Failed to remove file" << destinationPath;
                }

                break;
            }

            LOG_INFO << "Added plugin" << fullpath;
            success = true;
        } while (false);

        return success;
    }

    bool PluginManager::doAddPlugin(const QString &filepath) {
        bool added = false;
        auto plugin = loadPlugin(filepath);
        if (plugin) {
            const int size = (int)m_PluginsList.size();

            beginInsertRows(QModelIndex(), size, size);
            {
                m_PluginsList.push_back(plugin);
            }
            endInsertRows();

            const int pluginID = plugin->getPluginID();
            m_PluginsDict.insert(pluginID, plugin);

            added = true;
        }

        return added;
    }

    std::shared_ptr<PluginWrapper> PluginManager::loadPlugin(const QString &filepath) {
        LOG_INFO << filepath;
        std::shared_ptr<PluginWrapper> result;

        try {
            QPluginLoader loader(filepath);
            QObject *plugin = loader.instance();
            if (plugin) {
                XpiksPluginInterface *xpiksPlugin = qobject_cast<XpiksPluginInterface *>(plugin);

                if (xpiksPlugin) {
                    result = instantiatePlugin(filepath, xpiksPlugin);
                } else {
                    LOG_DEBUG << "Not Xpiks Plugin:" << filepath;
                }
            } else {
                LOG_WARNING << loader.errorString();
            }
        } catch(...) {
            LOG_WARNING << "Exception while loading" << filepath;
        }

        return result;
    }

    std::shared_ptr<PluginWrapper> PluginManager::instantiatePlugin(const QString &filepath, XpiksPluginInterface *plugin) {
        const int pluginID = getNextPluginID();
        LOG_INFO << "ID:" << pluginID << "name:" << plugin->getPrettyName() << "version:" << plugin->getVersionString() << "filepath:" << filepath;

        std::shared_ptr<PluginWrapper> pluginWrapper(new PluginWrapper(filepath, plugin, pluginID, &m_UIProvider));

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
