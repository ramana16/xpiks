/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "locallibraryloadsaveitem.h"
#include <QFile>
#include <QHash>
#include "../Suggestion/locallibrary.h"
#include "../Common/defines.h"

namespace Maintenance {
    LocalLibraryLoadSaveItem::LocalLibraryLoadSaveItem(Suggestion::LocalLibrary *localLibrary, LoadOption option):
        m_LocalLibrary(localLibrary),
        m_LoadOption(option)
    {        
        Q_ASSERT(localLibrary != nullptr);
    }

    void LocalLibraryLoadSaveItem::processJob() {
        LOG_DEBUG << "#";

        if (m_LoadOption == Save) {
            saveLibrary();
        } else if (m_LoadOption == Load) {
            readLibrary();
        } else if (m_LoadOption == Clean) {
            cleanupLibrary();
        }
    }

    void LocalLibraryLoadSaveItem::readLibrary() {
        LOG_DEBUG << "#";

        auto &filepath = m_LocalLibrary->getLibraryPath();

        QFile file(filepath);
        if (file.open(QIODevice::ReadOnly)) {
            QHash<QString, Suggestion::LocalArtworkData> dict;

            QDataStream in(&file);   // read the data
            in >> dict;
            file.close();

            m_LocalLibrary->swap(dict);
        } else {
            LOG_WARNING << "Failed to open" << filepath;
        }
    }

    void LocalLibraryLoadSaveItem::saveLibrary() {
        LOG_DEBUG << "#";
        m_LocalLibrary->saveToFile();
    }

    void LocalLibraryLoadSaveItem::cleanupLibrary() {
        LOG_DEBUG << "#";
        m_LocalLibrary->cleanupTrash();
    }
}
