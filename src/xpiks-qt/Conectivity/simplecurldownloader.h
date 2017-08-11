/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SIMPLECURLDOWNLOADER_H
#define SIMPLECURLDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTemporaryFile>

namespace Models {
    class ProxySettings;
}

namespace Conectivity {
    class SimpleProgressReporter : public QObject
    {
        Q_OBJECT
    public:
        SimpleProgressReporter(void *curl);

    public:
        bool cancelRequested() const { return m_Cancel; }

    signals:
        void progressChanged(double percentsDone);

    public slots:
        void cancelHandler();

    private:
        void *m_Curl;
        volatile bool m_Cancel;
    };

    class SimpleCurlDownloader : public QObject
    {
        Q_OBJECT
    public:
        explicit SimpleCurlDownloader(const QString &resource, QObject *parent = 0);
        virtual ~SimpleCurlDownloader();

    public:
        const QString &getErrorString() const { return m_ErrorString; }
        QString getDownloadedPath() const { return m_TempFile.fileName(); }

    public:
        void dispose() { emit stopped(); }
        bool downloadFileSync();
        void setRawHeaders(const QStringList &headers);
        void setProxySettings(Models::ProxySettings *proxySettings);

    public slots:
        void process();

    signals:
        void downloadFinished(bool success);
        void stopped();
        void cancelRequested();

    private:
        bool doDownloadFile();

    private:
        QTemporaryFile m_TempFile;
        QString m_RemoteResource;
        QStringList m_RawHeaders;
        QString m_ErrorString;
        Models::ProxySettings *m_ProxySettings;
        bool m_VerifySSL;
    };
}

#endif // SIMPLECURLDOWNLOADER_H
