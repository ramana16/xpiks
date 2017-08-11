/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAREADINGWORKER_H
#define METADATAREADINGWORKER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <QByteArray>
#include <QHash>
#include <QSize>
#include "importdataresult.h"
#include "imetadatareader.h"

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class BackupSaverService;

    class MetadataReadingWorker : public QObject, public IMetadataReader
    {
        Q_OBJECT
    public:
        explicit MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                       Models::SettingsModel *settingsModel, const QVector<QPair<int, int> > &rangesToUpdate);
        virtual ~MetadataReadingWorker();

    signals:
        void stopped();
        void finished(bool success);

    public slots:
        void process();
        void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    public:
        virtual const QHash<QString, ImportDataResult> &getImportResult() const override { return m_ImportResult; }
        virtual const QVector<Models::ArtworkMetadata *> &getItemsToRead() const override { return m_ItemsToRead; }
        virtual const QVector<QPair<int, int> > &getRangesToUpdate() const override { return m_RangesToUpdate; }

    private:
        void initWorker();
        QStringList createArgumentsList();
        void parseExiftoolOutput(const QByteArray &output);
        void readBackupsAndSizes(bool exiftoolSuccess);
        void readSizes();

    private:
        QVector<Models::ArtworkMetadata *> m_ItemsToRead;
        QHash<QString, ImportDataResult> m_ImportResult;
        QProcess *m_ExiftoolProcess;
        QVector<QPair<int, int> > m_RangesToUpdate;
        Models::SettingsModel *m_SettingsModel;
    };
}

#endif // METADATAREADINGWORKER_H
