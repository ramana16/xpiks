/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SUGGESTIONQUERYENGINE_H
#define SUGGESTIONQUERYENGINE_H

#include <QObject>
#include <QStringList>
#include <QJsonArray>
#include "suggestionqueryenginebase.h"

namespace Models {
    class SettingsModel;
}

namespace Suggestion {
    class SuggestionArtwork;

    class ShutterstockQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        ShutterstockQueryEngine(int engineID, Models::SettingsModel *settingsModel);

    public:
        virtual void submitQuery(const SearchQuery &query) override;
        virtual QString getName() const override { return tr("Shutterstock"); }

    private slots:
        void requestFinishedHandler(bool success);

    private:
        void parseResponse(const QJsonArray &jsonArray,
                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks);
        QUrl buildQuery(const SearchQuery &query) const;
        QString resultsTypeToString(Common::flag_t queryFlags) const;

    private:
        QString m_ClientId;
        QString m_ClientSecret;
    };
}

#endif // SUGGESTIONQUERYENGINE_H
