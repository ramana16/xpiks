/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTOCOMPLETEWORKER_H
#define AUTOCOMPLETEWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "completionquery.h"
#include <src/libfaceapi.hpp>

namespace Helpers {
    class AsyncCoordinator;
}

namespace KeywordsPresets {
    class IPresetsManager;
}

namespace AutoComplete {
    class AutoCompleteWorker :
            public QObject,
            public Common::ItemProcessingWorker<CompletionQuery>
    {
        Q_OBJECT
    public:
        explicit AutoCompleteWorker(Helpers::AsyncCoordinator *initCoordinator, KeywordsPresets::IPresetsManager *presetsManager, QObject *parent = 0);
        virtual ~AutoCompleteWorker();

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<CompletionQuery> &item) override;

    private:
        void generateCompletions(std::shared_ptr<CompletionQuery> &item);
        void updateCompletions(std::shared_ptr<CompletionQuery> &item);

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
        Helpers::AsyncCoordinator *m_InitCoordinator;
        KeywordsPresets::IPresetsManager *m_PresetsManager;
        Souffleur *m_Soufleur;
        const int m_CompletionsCount;
    };
}

#endif // AUTOCOMPLETEWORKER_H
