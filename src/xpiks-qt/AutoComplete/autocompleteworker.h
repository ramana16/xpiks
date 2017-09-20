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
#include "libfacecompletionengine.h"
#include "presetscompletionengine.h"

namespace Helpers {
    class AsyncCoordinator;
}

namespace KeywordsPresets {
    class PresetKeywordsModel;
}

namespace AutoComplete {
    class KeywordsAutoCompleteModel;

    class AutoCompleteWorker :
            public QObject,
            public Common::ItemProcessingWorker<CompletionQuery>
    {
        Q_OBJECT
    public:
        explicit AutoCompleteWorker(Helpers::AsyncCoordinator *initCoordinator,
                                    KeywordsAutoCompleteModel *autoCompleteModel,
                                    KeywordsPresets::PresetKeywordsModel *presetsManager,
                                    QObject *parent = 0);
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
        LibFaceCompletionEngine m_FaceCompletionEngine;
        PresetsCompletionEngine m_PresetsCompletionEngine;
        Helpers::AsyncCoordinator *m_InitCoordinator;
        KeywordsAutoCompleteModel *m_AutoCompleteModel;
        KeywordsPresets::PresetKeywordsModel *m_PresetsManager;
    };
}

#endif // AUTOCOMPLETEWORKER_H
