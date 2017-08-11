/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBRARYQUERYWORKER_H
#define LIBRARYQUERYWORKER_H

#include <QObject>
#include <vector>
#include <memory>
#include <QStringList>

namespace Suggestion {
    class LocalLibrary;
    class SuggestionArtwork;

    class LibraryQueryWorker : public QObject
    {
        Q_OBJECT
    public:
        LibraryQueryWorker(Suggestion::LocalLibrary *localLibrary, const QStringList &query, int maxResults);

        void doShutdown() { emit stopped(); }
        std::vector<std::shared_ptr<SuggestionArtwork> > &getResults() { return m_Results; }

    signals:
        void stopped();
        void resultsFound();

    public slots:
        void process();
        void cancel();

    private:
        Suggestion::LocalLibrary *m_LocalLibrary;
        std::vector<std::shared_ptr<SuggestionArtwork> > m_Results;
        QStringList m_Query;
        int m_MaxResults;
        volatile bool m_Cancel;
    };
}

#endif // LIBRARYQUERYWORKER_H
