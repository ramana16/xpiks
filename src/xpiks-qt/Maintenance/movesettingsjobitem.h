/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MOVESETTINGSJOBITEM_H
#define MOVESETTINGSJOBITEM_H

#include "imaintenanceitem.h"

namespace Models {
    class SettingsModel;
}

namespace Maintenance {
    class MoveSettingsJobItem: public IMaintenanceItem
    {
    public:
        MoveSettingsJobItem(Models::SettingsModel *settingsModel);

    public:
        virtual void processJob() override;

    private:
        void doMoveSettings();

    private:
        Models::SettingsModel *m_SettingsModel;
    };
}

#endif // MOVESETTINGSJOBITEM_H
