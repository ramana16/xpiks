/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "presetgroupsmodel.h"
#include <QQmlEngine>
#include "../Common/defines.h"
#include "presetkeywordsmodelconfig.h"
#include "presetkeywordsmodel.h"

namespace KeywordsPresets {
    PresetGroupsModel::PresetGroupsModel(PresetKeywordsModel *presetsModel):
        m_PresetsModel(presetsModel)
    {
        Q_ASSERT(presetsModel != nullptr);
    }

    bool PresetGroupsModel::tryGetGroupIndexById(int groupID, size_t &index) {
        QReadLocker locker(&m_GroupsLock);
        Q_UNUSED(locker);

        bool result = tryGetGroupIndexByIdUnsafe(groupID, index);
        return result;
    }

    bool PresetGroupsModel::isGroupIDValid(int groupID) {
        size_t dummy;
        bool found = tryGetGroupIndexById(groupID, dummy);
        return found;
    }

    void PresetGroupsModel::findOrRegisterGroup(const QString &name, int &groupID) {
        LOG_INFO << name;
        bool added = false;
        int indexOfLast = 0;

        {
            QWriteLocker locker(&m_GroupsLock);
            Q_UNUSED(locker);

            size_t index;
            if (!tryFindGroupIndexByNameUnsafe(name, index)) {
                const QString nameToAdd = name.simplified();
                int groupIDToAdd = DEFAULT_GROUP_ID;

                const int maxID = getLargestID();
                LOG_DEBUG << "max ID:" << maxID;
                groupIDToAdd = maxID + 1;

                Q_ASSERT(m_GroupsList.size() == m_FilteredGroups.size());

                m_GroupsList.emplace_back(nameToAdd, groupIDToAdd);
                m_FilteredGroups.emplace_back(nullptr);

                index = m_GroupsList.size() - 1;
                indexOfLast = (int)index;
                added = true;
            }

            groupID = m_GroupsList[index].m_GroupID;
        }

        if (added) {
            beginInsertRows(QModelIndex(), indexOfLast, indexOfLast);
            {
                // multithreading hack
            }
            endInsertRows();

            emit groupNamesChanged();
            emit backupRequested();
        }
    }

    bool PresetGroupsModel::tryGetGroupIndexByIdUnsafe(int groupID, size_t &index) {
        bool found = false;
        const size_t size = m_GroupsList.size();

        for (size_t i = 0; i < size; i++) {
            if (m_GroupsList[i].m_GroupID == groupID) {
                index = i;
                found = true;
                break;
            }
        }

        return found;
    }

    bool PresetGroupsModel::tryFindGroupIndexByNameUnsafe(const QString &name, size_t &index) {
        LOG_INFO << name;
        size_t size = m_GroupsList.size();
        bool found = false;

        for (size_t i = 0; i < size; ++i) {
            auto &group = m_GroupsList[i];

            if (QString::compare(group.m_Name, name, Qt::CaseInsensitive) == 0) {
                index = i;
                found = true;
                break;
            }
        }

        return found;
    }

    int PresetGroupsModel::addGroup(const QString &name) {
        int groupID = 0;

        QString sanitizedName = name.trimmed();
        if (sanitizedName.isEmpty()) {
            return -1;
        }

        findOrRegisterGroup(sanitizedName, groupID);
        return groupID;
    }

    int PresetGroupsModel::findGroupIndexById(int groupID) {
        size_t index;
        bool found = tryGetGroupIndexById(groupID, index);

        LOG_DEBUG << "group" << groupID << (found ? "found" : "not found") << index;

        return (found ? (int)index : -1);
    }

    int PresetGroupsModel::findGroupIdByIndex(int groupIndex) {
        if ((groupIndex < 0) || (groupIndex >= (int)m_GroupsList.size())) {
            return DEFAULT_GROUP_ID;
        }

        int groupID = getGroupID(groupIndex);
        return groupID;
    }

    QObject *PresetGroupsModel::getGroupModel(int index) {
        LOG_DEBUG << index;

        if ((index < 0) || (index >= (int)m_GroupsList.size())) {
            return nullptr;
        }

        Q_ASSERT(m_GroupsList.size() == m_FilteredGroups.size());
        Q_ASSERT(m_PresetsModel != nullptr);

        if (!m_FilteredGroups[index]) {
            m_FilteredGroups[index].reset(new PresetKeywordsGroupModel(m_GroupsList[index].m_GroupID));
            m_FilteredGroups[index]->setSourceModel(m_PresetsModel);
        }

        QObject *result = m_FilteredGroups[index].get();
        QQmlEngine::setObjectOwnership(result, QQmlEngine::CppOwnership);
        return result;
    }

    QObject *PresetGroupsModel::getDefaultGroupModel() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_PresetsModel != nullptr);

        if (!m_DefaultGroup) {
            m_DefaultGroup.reset(new PresetKeywordsGroupModel(DEFAULT_GROUP_ID));
            m_DefaultGroup->setSourceModel(m_PresetsModel);
        }

        QObject *result = m_DefaultGroup.get();
        QQmlEngine::setObjectOwnership(result, QQmlEngine::CppOwnership);
        return result;
    }

    bool PresetGroupsModel::canAddGroup(const QString &name) {
        const QString groupName = name.simplified();

        QReadLocker locker(&m_GroupsLock);
        Q_UNUSED(locker);

        size_t dummy;
        bool found = tryFindGroupIndexByNameUnsafe(groupName, dummy);
        return !found;
    }

    QStringList PresetGroupsModel::retrieveGroupNames() {
        QReadLocker locker(&m_GroupsLock);
        Q_UNUSED(locker);

        QStringList result;

        result.reserve((int)m_FilteredGroups.size() + 1);
        result.append(QObject::tr("Default"));

        for (auto& group: m_GroupsList) {
            result.append(group.m_Name);
        }

        return result;
    }

    void PresetGroupsModel::initGroups(const std::vector<PresetGroupData> &groupsData) {
        LOG_DEBUG << groupsData.size() << "group(s)";
        if (groupsData.empty()) { return; }

        std::vector<GroupModel> groups;
        groups.reserve(groupsData.size());

        for (const auto &item: groupsData) {
            groups.emplace_back(item.m_Name, item.m_ID);
        }

        beginResetModel();
        {
            m_GroupsList.swap(groups);
            m_FilteredGroups.clear();
            m_FilteredGroups.resize(m_GroupsList.size());
        }
        endResetModel();
    }

    int PresetGroupsModel::getLargestID() const {
        int id = 0;

        for (auto &group: m_GroupsList) {
            if (group.m_GroupID > id) {
                id = group.m_GroupID;
            }
        }

        return id;
    }

    int PresetGroupsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_GroupsList.size();
    }

    QVariant PresetGroupsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= (int)m_GroupsList.size())) { return QVariant(); }

        switch(role) {
        case NameRole: return m_GroupsList[row].m_Name;
        case IdRole: return m_GroupsList[row].m_GroupID;
        }

        return QVariant();
    }

    QHash<int, QByteArray> PresetGroupsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "gname";
        roles[IdRole] = "gid";
        return roles;
    }
}
