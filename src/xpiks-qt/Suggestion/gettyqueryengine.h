/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef GETTYQUERYENGINE_H
#define GETTYQUERYENGINE_H

#include <QString>
#include "suggestionqueryenginebase.h"

namespace Models {
    class SettingsModel;
}

namespace Suggestion {
    class GettyQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        GettyQueryEngine(int engineID, Models::SettingsModel *settingsModel);

    public:
        virtual void submitQuery(const SearchQuery &query) override;
        virtual QString getName() const override { return tr("iStock"); }

    private slots:
        void requestFinishedHandler(bool success);

    private:
        void parseResponse(const QJsonObject &jsonObject, int count,
                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks);
        void parseUrl(const QJsonValue &previewObject, QString &url);
        void parseKeywords(const QJsonValue &keywordsObject, QStringList &keywords);
        QUrl buildQuery(const SearchQuery &query) const;
        QString resultsTypeToString(Common::flag_t queryFlags) const;

    private:
        QString m_GettyImagesAPIKey;
    };
}

#endif // GETTYQUERYENGINE_H
