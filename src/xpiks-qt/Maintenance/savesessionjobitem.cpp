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

#include "../Models/sessionmanager.h"
#include "savesessionjobitem.h"

namespace Maintenance {
    SaveSessionJobItem::SaveSessionJobItem(std::unique_ptr<MetadataIO::SessionSnapshot> &sessionSnapshot, Models::SessionManager *sessionManager):
        m_SessionSnapshot(std::move(sessionSnapshot)),
        m_SessionManager(sessionManager)
    {
        Q_ASSERT(m_SessionManager != NULL);
    }

    void SaveSessionJobItem::processJob() {
        LOG_DEBUG << "#";
        doSaveSession();
    }

    void SaveSessionJobItem::doSaveSession() {
        auto snapshot = m_SessionSnapshot->getSnapshot();
        m_SessionManager->saveToFile(snapshot);
    }
}
