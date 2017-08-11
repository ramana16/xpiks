/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAWRITINGWORKER_H
#define METADATAWRITINGWORKER_H

#include <QObject>
#include <QVector>
#include <QProcess>
#include "imetadatawriter.h"

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class MetadataWritingWorker : public QObject, public IMetadataWriter
    {
        Q_OBJECT
    public:
        explicit MetadataWritingWorker(const QVector<Models::ArtworkMetadata*> &itemsToWrite,
                                       Models::SettingsModel *settingsModel,
                                       bool useBackups);
        virtual ~MetadataWritingWorker();

    signals:
        void stopped();
        void finished(bool success);

    public:
        virtual const QVector<Models::ArtworkMetadata *> &getItemsToWrite() const override { return m_ItemsToWrite; }

    public slots:
        void process();
        //void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        void initWorker();
        QStringList createArgumentsList(const QString &jsonFilePath);

    private:
        QVector<Models::ArtworkMetadata*> m_ItemsToWrite;
        QProcess *m_ExiftoolProcess;
        Models::SettingsModel *m_SettingsModel;
        bool m_UseBackups;
    };
}

#endif // METADATAWRITINGWORKER_H
