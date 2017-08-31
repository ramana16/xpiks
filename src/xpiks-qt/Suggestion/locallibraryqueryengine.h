/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOCALLIBRARYQUERYENGINE_H
#define LOCALLIBRARYQUERYENGINE_H

#include <QObject>
#include <QString>
#include "suggestionqueryenginebase.h"
#include "locallibraryquery.h"

namespace MetadataIO {
    class MetadataIOService;
}

namespace Suggestion {
    class LocalLibraryQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        LocalLibraryQueryEngine(int engineID, MetadataIO::MetadataIOService *metadataIOService);

        // ISuggestionQueryEngine interface
    public:
        virtual int getMaxResults() const override { return 200; }
        virtual void submitQuery(const SearchQuery &query) override;
        virtual QString getName() const override { return tr("Local files"); }

    private slots:
        void resultsFoundHandler();

    private:
        LocalLibraryQuery m_Query;
        MetadataIO::MetadataIOService *m_MetadataIOService;
    };
}

#endif // LOCALLIBRARYQUERYENGINE_H
