/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WRITINGORCHESTRATOR_H
#define WRITINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QAtomicInt>
#include "imetadatawriter.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class WritingOrchestrator : public QObject, public IMetadataWriter
    {
        Q_OBJECT
    public:
        explicit WritingOrchestrator(const QVector<Models::ArtworkMetadata*> &itemsToWrite, QObject *parent = 0);
        virtual ~WritingOrchestrator();

    public:
        virtual const QVector<Models::ArtworkMetadata *> &getItemsToWrite() const override { return m_ItemsToWrite; }
        void startWriting();

    signals:
        void allStarted();
        void allFinished(bool anyError);

    public slots:
        void dismiss();

    private slots:
        void onWorkerFinished(bool anyError);

    private:
        QVector<Models::ArtworkMetadata*> m_ItemsToWrite;
        QVector<QVector<Models::ArtworkMetadata *> > m_SlicedItemsToWrite;
        int m_ThreadsCount;
        QAtomicInt m_FinishedCount;
        volatile bool m_AnyError;
    };
}

#endif // WRITINGORCHESTRATOR_H
