/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOCALLIBRARYLOADSAVEITEM_H
#define LOCALLIBRARYLOADSAVEITEM_H

#include <QString>
#include "imaintenanceitem.h"

namespace Suggestion {
    class LocalLibrary;
}

namespace Maintenance {
    class LocalLibraryLoadSaveItem: public IMaintenanceItem
    {
    public:
        enum LoadOption {
            Load, Save, Clean
        };

    public:
        LocalLibraryLoadSaveItem(Suggestion::LocalLibrary *localLibrary, LoadOption option);

        // IMaintenanceItem interface
    public:
        virtual void processJob() override;

    private:
        void readLibrary();
        void saveLibrary();
        void cleanupLibrary();

    private:
        Suggestion::LocalLibrary *m_LocalLibrary;
        LoadOption m_LoadOption;
    };
}

#endif // LOCALLIBRARYLOADSAVEITEM_H
