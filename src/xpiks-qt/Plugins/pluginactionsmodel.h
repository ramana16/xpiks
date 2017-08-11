/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
