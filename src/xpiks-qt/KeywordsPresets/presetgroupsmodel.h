/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETGROUPSMODEL_H
#define PRESETGROUPSMODEL_H

#include <QString>
#include <QAbstractListModel>
#include <QReadWriteLock>
#include <vector>
#include <memory>

namespace KeywordsPresets {
    struct GroupModel {
        GroupModel(const QString &name, int groupID):
            m_Name(name),
            m_GroupID(groupID)
        { }

        QString m_Name;
        int m_GroupID;
    };

    struct PresetGroupData;
    class PresetKeywordsGroupModel;
    class PresetKeywordsModel;

    class PresetGroupsModel: public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(QStringList groupNames READ retrieveGroupNames NOTIFY groupNamesChanged)
    public:
        PresetGroupsModel(PresetKeywordsModel *presetsModel);

    private:
        enum PresetGroups_Roles {
            NameRole = Qt::DisplayRole + 1,
            IdRole
        };

    public:
        bool tryGetGroupIndexById(int groupID, size_t &index);
        bool isGroupIDValid(int groupID);
        void findOrRegisterGroup(const QString &name, int &groupID);

    private:
        bool tryGetGroupIndexByIdUnsafe(int groupID, size_t &index);
        bool tryFindGroupIndexByNameUnsafe(const QString &name, size_t &index);

    public:
        Q_INVOKABLE int addGroup(const QString &name);
        Q_INVOKABLE int findGroupIndexById(int groupID);
        Q_INVOKABLE int findGroupIdByIndex(int groupIndex);
        Q_INVOKABLE QObject *getGroupModel(int index);
        Q_INVOKABLE QObject *getDefaultGroupModel();
        Q_INVOKABLE int getGroupsCount() { return (int)m_GroupsList.size(); }
        Q_INVOKABLE bool canAddGroup(const QString &name);

    public:
        const std::vector<GroupModel> &getGroups() const { return m_GroupsList; }
        QStringList retrieveGroupNames();

    public:
        void initGroups(const std::vector<PresetGroupData> &groupsData);
        int getGroupID(size_t index) { Q_ASSERT(index < m_GroupsList.size()); return m_GroupsList[index].m_GroupID; }

    private:
        int getLargestID() const;

    signals:
        void groupNamesChanged();
        void backupRequested();

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        QReadWriteLock m_GroupsLock;
        std::vector<GroupModel> m_GroupsList;
        std::vector<std::shared_ptr<PresetKeywordsGroupModel> > m_FilteredGroups;
        std::shared_ptr<PresetKeywordsGroupModel> m_DefaultGroup;
        PresetKeywordsModel *m_PresetsModel;
    };
}

#endif // PRESETGROUPSMODEL_H
