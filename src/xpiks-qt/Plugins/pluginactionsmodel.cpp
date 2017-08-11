/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

