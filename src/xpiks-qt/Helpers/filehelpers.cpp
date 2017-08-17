/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filehelpers.h"
#include <QRegExp>
#include <QFileInfo>
#include <QDir>
#include "../Common/defines.h"

QStringList Helpers::convertToVectorFilenames(const QStringList &item) {
    QStringList converted;
    converted.reserve(item.length()*2);

    foreach (const QString &item, item) {
        converted.append(convertToVectorFilenames(item));
    }

    return converted;
}

QStringList Helpers::convertToVectorFilenames(const QString &path) {
    QStringList result;

    QString base;
    if (path.endsWith(".jpg", Qt::CaseInsensitive) ||
            path.endsWith(".tif", Qt::CaseInsensitive)) {
        base = path.mid(0, path.size() - 4);
        result << (base + ".eps") << (base + ".ai");
    } else if (path.endsWith(".jpeg", Qt::CaseInsensitive) ||
               path.endsWith(".tiff", Qt::CaseInsensitive)) {
        base = path.mid(0, path.size() - 5);
        result << (base + ".eps") << (base + ".ai");
    }

    return result;
}

QString Helpers::getArchivePath(const QString &artworkPath) {
    QFileInfo fi(artworkPath);
    QString archiveName = fi.baseName() + ".zip";

    QString basePath = fi.absolutePath();
    QDir dir(basePath);
    QString archivePath = dir.filePath(archiveName);
    return archivePath;
}

QString Helpers::getImagePath(const QString &path) {
    QString result = path;

    QRegExp regExp("(.*)[.](ai|eps|zip)", Qt::CaseInsensitive);
    result.replace(regExp, "\\1.jpg");

    return result;
}

bool Helpers::ensureDirectoryExists(const QString &path) {
    bool anyError = false;

    if (!QDir(path).exists()) {
        LOG_INFO << "Creating" << path;
        if (!QDir().mkpath(path)) {
            anyError = true;
        }
    }

    return !anyError;
}
