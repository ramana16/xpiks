/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
