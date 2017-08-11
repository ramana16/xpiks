/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
