/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FOTOLIAQUERYENGINE_H
#define FOTOLIAQUERYENGINE_H

#include <QString>
#include "suggestionqueryenginebase.h"

namespace Models {
    class SettingsModel;
}

namespace Suggestion {
    class FotoliaQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        FotoliaQueryEngine(int engineID, Models::SettingsModel *settingsModel);

    public:
        virtual void submitQuery(const SearchQuery &query) override;
        virtual QString getName() const override { return tr("Fotolia"); }

    private slots:
        void requestFinishedHandler(bool success);

    private:
        void parseResponse(const QJsonObject &jsonObject, int count,
                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks);
        QUrl buildQuery(const QString &apiKey, const SearchQuery &query) const;
        QString resultsTypeToString(Common::flag_t queryFlags) const;

    private:
        QString m_FotoliaAPIKey;
    };
}

#endif // FOTOLIAQUERYENGINE_H
