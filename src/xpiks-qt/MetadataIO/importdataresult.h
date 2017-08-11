/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMPORTDATARESULT_H
#define IMPORTDATARESULT_H

#include <QStringList>
#include <QString>
#include <QSize>
#include <QHash>
#include <QDateTime>

namespace MetadataIO {
    struct ImportDataResult {
        QString FilePath;
        QString Title;
        QString Description;
        QStringList Keywords;
        QSize ImageSize;
        qint64 FileSize;
        QHash<QString, QString> BackupDict;
        QDateTime DateTimeOriginal;
    };
}

#endif // IMPORTDATARESULT_H
