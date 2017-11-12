/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FILENAMESHELPERS
#define FILENAMESHELPERS

#include <QStringList>

#ifdef CORE_TESTS
    #ifdef Q_OS_WIN
        #define DIRECTORY_PATH "C:/path/to/some/directory"
    #else
        #define DIRECTORY_PATH "/path/to/some/directory"
    #endif

    #define ARTWORK_JPG_PATTERN "artwork%2.jpg"

    #define ARTWORK_PATH DIRECTORY_PATH "_%1/" ARTWORK_JPG_PATTERN
    #define VECTOR_PATH DIRECTORY_PATH "_%1/artwork%2.eps"
#endif

namespace Models {
    class ArtworkMetadata;
}

namespace Helpers {
    QStringList convertToVectorFilenames(const QStringList &items);
    QStringList convertToVectorFilenames(const QString &path);
    QString getImagePath(const QString &path);
    QString getArchivePath(const QString &artworkPath);
    bool couldBeVideo(const QString &artworkPath);
    bool isVideoExtension(const QString &extension);
    bool isVectorExtension(const QString &extension);
    bool isImageExtension(const QString &extension);
    bool isSupportedExtension(const QString &extension);
    QString describeFileSize(qint64 filesizeBytes);
    bool ensureDirectoryExists(const QString &path);
    void extractFilesFromDirectory(const QString &directory, QStringList &filesList);
    void splitMediaFiles(const QStringList &rawFilenames, QStringList &filenames, QStringList &vectors);
}

#endif // FILENAMESHELPERS

