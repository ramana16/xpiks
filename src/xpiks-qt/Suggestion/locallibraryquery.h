/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOCALLIBRARYQUERY_H
#define LOCALLIBRARYQUERY_H

#include <QObject>
#include <QVector>
#include "searchquery.h"
#include "../MetadataIO/cachedartwork.h"

namespace Suggestion {
    class LocalLibraryQuery: public QObject {
        Q_OBJECT
    public:
        LocalLibraryQuery():
            QObject()
        { }

    public:
        void setSearchQuery(const SearchQuery &value) { m_SearchQuery = value; }
        const SearchQuery &getSearchQuery() const { return m_SearchQuery; }
        QVector<MetadataIO::CachedArtwork> &getResults() { return m_Results; }

    public:
        void clear() { m_Results.clear(); }

    public:
        void notifyResultsReady() { emit resultsReady(); }

    signals:
        void resultsReady();

    private:
        QVector<MetadataIO::CachedArtwork> m_Results;
        SearchQuery m_SearchQuery;
    };
}

#endif // LOCALLIBRARYQUERY_H
