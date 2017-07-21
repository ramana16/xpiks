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

#ifndef SWITCHERMODEL_H
#define SWITCHERMODEL_H

#include <QObject>
#include "../Conectivity/switcherconfig.h"
#include "../Common/baseentity.h"

namespace Models {
    class SwitcherModel: public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isDonationCampaign1Active READ getIsDonationCampaign1Active NOTIFY switchesUpdated)
    public:
        SwitcherModel(QObject *parent=nullptr);

    public:
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;
        void updateConfigs();

    public:
        bool getIsDonationCampaign1Active() { return m_Config.isSwitchOn(Conectivity::SwitcherConfig::DonateCampaign1); }

    private slots:
        void configUpdated();

    signals:
        void switchesUpdated();

    private:
        Conectivity::SwitcherConfig m_Config;
    };
}

#endif // SWITCHERMODEL_H
