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

#include "pluginactionsmodel.h"
#include <QHash>
#include "ipluginaction.h"
#include "../Common/defines.h"

namespace Plugins {
    PluginActionsModel::PluginActionsModel(const std::vector<std::shared_ptr<IPluginAction> > &actions, int pluginID, QObject *parent) :
        QAbstractListModel(parent),
        m_PluginActions(actions),
        m_PluginID(pluginID)
    {
        LOG_INFO << "initialized" << m_PluginActions.size() << "action(s)";
    }

    int PluginActionsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_PluginActions.size();
    }

    QVariant PluginActionsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_PluginActions.size()) { return QVariant(); }

        auto &action = m_PluginActions.at(row);

        switch (role) {
            case ActionNameRole:
                return action->getActionName();
            case ActionCodeRole:
                return action->getActionID();
            case PluginIDRole:
                return m_PluginID;
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> PluginActionsModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[ActionNameRole] = "aname";
        roles[ActionCodeRole] = "acode";
        roles[PluginIDRole] = "pluginID";
        return roles;
    }
}

