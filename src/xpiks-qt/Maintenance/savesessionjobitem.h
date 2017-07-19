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

#ifndef SAVESESSIONJOBITEM_H
#define SAVESESSIONJOBITEM_H

#include "imaintenanceitem.h"
#include "../MetadataIO/artworkmetadatasnapshot.h"

namespace Models {
    class SessionManager;
}

namespace Maintenance {
    class SaveSessionJobItem : public IMaintenanceItem
    {
    public:
        SaveSessionJobItem(std::unique_ptr<MetadataIO::SessionSnapshot> &sessionSnapshot, class Models::SessionManager *sessionManager);

    public:
        virtual void processJob() override;

    private:
        void doSaveSession();

    private:
        std::unique_ptr<MetadataIO::SessionSnapshot> m_SessionSnapshot;
        class Models::SessionManager *m_SessionManager;
    };
}

#endif // SAVESESSIONJOBITEM_H
