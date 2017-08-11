/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXIV2READINGWORKER_H
#define EXIV2READINGWORKER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include "importdataresult.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class Exiv2ReadingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit Exiv2ReadingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToRead, QObject *parent = 0);
        virtual ~Exiv2ReadingWorker();

    public:
        const QHash<QString, ImportDataResult> &getImportResult() const { return m_ImportResult; }
        int getWorkerIndex() const { return m_WorkerIndex; }

    public:
        void dismiss() { emit stopped(); }

    public slots:
        void process();
        void cancel();

    signals:
        void stopped();
        void finished(bool anyError);

    private:
        bool readMetadata(Models::ArtworkMetadata *artwork, ImportDataResult &importResult);

    private:
        QVector<Models::ArtworkMetadata *> m_ItemsToRead;
        QHash<QString, ImportDataResult> m_ImportResult;
        int m_WorkerIndex;
        volatile bool m_Stopped;
    };
}

#endif // EXIV2READINGWORKER_H
