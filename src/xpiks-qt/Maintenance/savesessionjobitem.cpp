/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../Models/sessionmanager.h"
#include "../MetadataIO/artworkssnapshot.h"
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
        auto &snapshot = m_SessionSnapshot->getSnapshot();
        auto &directories = m_SessionSnapshot->getDirectoriesSnapshot();
        m_SessionManager->saveToFile(snapshot, directories);
    }
}
