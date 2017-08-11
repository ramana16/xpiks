/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXIV2WRITINGWORKER_H
#define EXIV2WRITINGWORKER_H

#include <QObject>
#include <QVector>

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class Exiv2WritingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit Exiv2WritingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToWrite, QObject *parent = 0);

    public:
        int getWorkerIndex() const { return m_WorkerIndex; }
        void dismiss() { emit stopped(); }

    public slots:
        void process();
        void cancel();

    signals:
        void stopped();
        void finished(bool anyError);

    private:
        void writeMetadata(Models::ArtworkMetadata *artwork);

    private:
        QVector<Models::ArtworkMetadata*> m_ItemsToWrite;
        int m_WorkerIndex;
        volatile bool m_Stopped;
    };
}

#endif // EXIV2WRITINGWORKER_H
