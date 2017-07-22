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

#ifndef SWITCHERCONFIG_H
#define SWITCHERCONFIG_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QReadWriteLock>
#include "../Models/abstractconfigupdatermodel.h"

namespace Conectivity {
    class SwitcherConfig: public Models::AbstractConfigUpdaterModel
    {
        Q_OBJECT
    public:
        SwitcherConfig(QObject *parent=nullptr);

    public:
        void initializeConfigs();
        bool isSwitchOn(int switchKey);

    signals:
        void switchesUpdated();

        // AbstractConfigUpdaterModel interface
    protected:
        virtual bool processLocalConfig(const QJsonDocument &document) override;
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) override;

        // CompareValuesJson interface
    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2) override;

    private:
        void parseSwitches(const QJsonObject &object);

    public:
        enum Switches_Keys {
            DonateCampaign1=0,
            DonateCampaign1Stage2,
            SequentialSuggestionPreviews
        };

    private:
        QReadWriteLock m_RwLock;
        QHash<int, bool> m_SwitchesHash;
    };
}

#endif // SWITCHERCONFIG_H
