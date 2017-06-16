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
