/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xpkscleanupjob.h"
#include <QFile>
#include <QDirIterator>
#include "../Common/defines.h"
#include "../Helpers/constants.h"

namespace Maintenance {
    XpksCleanupJob::XpksCleanupJob(const QString directoryPath):
        m_DirectoryPath(directoryPath)
    {
    }

    void XpksCleanupJob::processJob() {
        LOG_DEBUG << m_DirectoryPath;

        QDirIterator it(m_DirectoryPath, QDirIterator::NoIteratorFlags);
        while (it.hasNext()) {
            QString filepath = it.next();
            if (filepath.endsWith(Constants::METADATA_BACKUP_EXTENSION, Qt::CaseInsensitive)) {
                QFile file(filepath);
                bool success = file.remove();
                LOG_DEBUG << "removing" << filepath << "result:" << success;
            }
        }
    }
}
