/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ziphelper.h"
#include <QFileInfo>
#include <quazip/JlCompress.h>
#include "filehelpers.h"
#include "../Common/defines.h"

namespace Helpers {
    QStringList zipFiles(QStringList filepathes) {
        QString zipFilePath;
        zipArtworkAndVector(filepathes, zipFilePath);
        return filepathes;
    }

    bool zipArtworkAndVector(const QStringList &filepathes, QString &zipFilePath) {
        QString anyFile = filepathes.first();
        QString archivePath = getArchivePath(anyFile);

        bool result = false;
        try {
            result = JlCompress::compressFiles(archivePath, filepathes);
        } catch (...) {
            LOG_WARNING << "Exception while zipping with QuaZip";
        }

        if (!result) {
            LOG_WARNING << "Failed to create zip" << archivePath;
        }

        zipFilePath = archivePath;
        return result;
    }
}


