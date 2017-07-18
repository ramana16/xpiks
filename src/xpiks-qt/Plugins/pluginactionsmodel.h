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

#ifndef PLUGINACTIONSMODEL_H
#define PLUGINACTIONSMODEL_H

#include <QAbstractListModel>
#include <vector>
#include <memory>

namespace Plugins {
    class IPluginAction;

    class PluginActionsModel: public QAbstractListModel
    {
        Q_OBJECT
    public:
        PluginActionsModel(const std::vector<std::shared_ptr<IPluginAction> > &actions, int pluginID, QObject *parent=0);

    public:
        enum PluginActionsModel_Roles {
            ActionNameRole = Qt::UserRole + 1,
            ActionCodeRole,
            PluginIDRole
        };

    public:
        int size() const { return (int)m_PluginActions.size(); }

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        std::vector<std::shared_ptr<IPluginAction> > m_PluginActions;
        int m_PluginID;
    };
}

#endif // PLUGINACTIONSMODEL_H
