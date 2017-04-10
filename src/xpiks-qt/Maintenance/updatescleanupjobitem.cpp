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

#include <QString>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFile>
#include "../Common/defines.h"
#include "updatescleanupjobitem.h"

namespace Maintenance {
    UpdatesCleanupJobItem::UpdatesCleanupJobItem()
    {

    }

    void UpdatesCleanupJobItem::processJob() {
#ifndef Q_OS_WIN
        Q_ASSERT(false);
#endif
        LOG_DEBUG << "#";
        doCleanUpdateArtifacts();
    }

    void UpdatesCleanupJobItem::doCleanUpdateArtifacts() {
        QString appDirPath = QCoreApplication::applicationDirPath();
        QDirIterator dirIt(appDirPath, QStringList() << "*.bak", QDir::Files, QDirIterator::Subdirectories);
        int count = 0;

        while (dirIt.hasNext()) {
            QString filePath = dirIt.next();
            QFile backedUpFile(filePath);

            if (backedUpFile.remove()) {
                LOG_INFO << "Removed prev update leftover:" << filePath;
            } else {
                LOG_WARNING << "Failed to remove leftover:" << filePath;
            }

            count++;
        }

        if (count == 0) {
            LOG_DEBUG << "No leftovers found";
        }
    }
}
