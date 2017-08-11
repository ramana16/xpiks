/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libraryqueryworker.h"
#include "../Suggestion/locallibrary.h"
#include "suggestionartwork.h"

namespace Suggestion {
    LibraryQueryWorker::LibraryQueryWorker(Suggestion::LocalLibrary *localLibrary, const QStringList &query, int maxResults) :
        m_LocalLibrary(localLibrary),
        m_Query(query),
        m_MaxResults(maxResults),
        m_Cancel(false)
    {
    }

    void LibraryQueryWorker::process() {
        m_LocalLibrary->searchArtworks(m_Query, m_Results, m_MaxResults);

        if (m_Cancel) {
            m_Results.clear();
            doShutdown();
        } else {
            emit resultsFound();
        }
    }

    void LibraryQueryWorker::cancel() {
        m_Cancel = true;
    }
}

