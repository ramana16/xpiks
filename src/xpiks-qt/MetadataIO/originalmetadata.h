/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORIGINALMETADATA_H
#define ORIGINALMETADATA_H

#include <QStringList>
#include <QString>
#include <QSize>
#include <QHash>
#include <QDateTime>

namespace MetadataIO {
    // metadata read directly from file by exiftool/etc.
    struct OriginalMetadata {
        QString m_FilePath;
        QString m_Title;
        QString m_Description;
        QStringList m_Keywords;
        QSize m_ImageSize;
        qint64 m_FileSize;
        QDateTime m_DateTimeOriginal;
    };
}

#endif // IMPORTDATARESULT_H
