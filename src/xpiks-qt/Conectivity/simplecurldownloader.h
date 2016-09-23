/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef SIMPLECURLDOWNLOADER_H
#define SIMPLECURLDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTemporaryFile>

namespace Conectivity {
    class SimpleCurlDownloader : public QObject
    {
        Q_OBJECT
    public:
        explicit SimpleCurlDownloader(const QString &resource, QObject *parent = 0);
        virtual ~SimpleCurlDownloader();

    public:
        const QString &getErrorString() const { return m_ErrorString; }
        const QString &getDownloadedPath() const { return m_TempFile.fileName(); }

    public:
        void dispose() { emit stopped(); }
        bool downloadFileSync();
        void setRawHeaders(const QStringList &headers);

    public slots:
        void process();

    signals:
        void downloadFinished(bool success);
        void stopped();

    private:
        bool doDownloadFile();

    private:
        QTemporaryFile m_TempFile;
        QString m_RemoteResource;
        QStringList m_RawHeaders;
        QString m_ErrorString;
        bool m_VerifySSL;
    };
}

#endif // SIMPLECURLDOWNLOADER_H
