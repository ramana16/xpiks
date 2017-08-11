/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "locallibraryqueryengine.h"

#include <QThread>
#include "libraryqueryworker.h"
#include "../Common/defines.h"
#define MAX_LOCAL_RESULTS 200

namespace Suggestion {
    LocalLibraryQueryEngine::LocalLibraryQueryEngine(int engineID, LocalLibrary *localLibrary):
        SuggestionQueryEngineBase(engineID),
        m_LocalLibrary(localLibrary)
    {
    }

    void LocalLibraryQueryEngine::submitQuery(const QStringList &queryKeywords, QueryResultsType resultsType) {
        LOG_DEBUG << queryKeywords;
        Q_UNUSED(resultsType);
        LibraryQueryWorker *worker = new LibraryQueryWorker(m_LocalLibrary, queryKeywords, MAX_LOCAL_RESULTS);
        QThread *thread = new QThread();
        worker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, worker, &LibraryQueryWorker::process);
        QObject::connect(worker, &LibraryQueryWorker::stopped, thread, &QThread::quit);

        QObject::connect(worker, &LibraryQueryWorker::stopped, worker, &LibraryQueryWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &LocalLibraryQueryEngine::cancelAllQueries,
                         worker, &LibraryQueryWorker::cancel);

        QObject::connect(worker, &LibraryQueryWorker::resultsFound,
                         this, &LocalLibraryQueryEngine::resultsFoundHandler);

        thread->start();
    }

    void LocalLibraryQueryEngine::resultsFoundHandler() {
        LibraryQueryWorker *worker = qobject_cast<LibraryQueryWorker*>(sender());
        Q_ASSERT(worker != nullptr);
        setResults(worker->getResults());
        worker->doShutdown();
        emit resultsAvailable();
    }
}
