/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TRANSLATIONWORKER_H
#define TRANSLATIONWORKER_H

#include <QObject>
#include <memory>
#include "../Common/itemprocessingworker.h"
#include "translationquery.h"

class LookupDictionary;

namespace Helpers {
    class AsyncCoordinator;
}

namespace Translation {
    class TranslationWorker :
            public QObject,
            public Common::ItemProcessingWorker<TranslationQuery>
    {
        Q_OBJECT
    public:
        explicit TranslationWorker(Helpers::AsyncCoordinator *initCoordinator, QObject *parent = 0);
        virtual ~TranslationWorker();

    public:
        void selectDictionary(const QString &dictionaryPath);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<TranslationQuery> &item) override;

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        void ensureDictionaryLoaded();

    private:
        std::unique_ptr<LookupDictionary> m_LookupDictionary;
        Helpers::AsyncCoordinator *m_InitCoordinator;
    };
}

#endif // TRANSLATIONWORKER_H
