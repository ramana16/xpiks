/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "fotoliaqueryengine.h"
#include <QObject>
#include <QUrl>
#include <QThread>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include "../Encryption/aes-qt.h"
#include "../Connectivity/simplecurlrequest.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"
#include "suggestionartwork.h"

namespace Suggestion {
    FotoliaQueryEngine::FotoliaQueryEngine(int engineID, Models::SettingsModel *settingsModel):
        SuggestionQueryEngineBase(engineID, settingsModel)
    {
        m_FotoliaAPIKey = "ad2954b4ee1e9686fbf8446f85e0c26edfae6003f51f49ca5559aed915879e733bbaf2003b3575bc0b96e682a30a69907c612865ec8f4ec2522131108a4a9f24467f1f83befc3d80201e5f906c761341";
    }

    void FotoliaQueryEngine::submitQuery(const SearchQuery &query) {
        LOG_INFO << query.m_SearchTerms;

        QString decodedAPIKey = Encryption::decodeText(m_FotoliaAPIKey, "MasterPassword");

        QUrl url = buildQuery(decodedAPIKey, query);

        auto *settings = getSettingsModel();
        auto *proxySettings = settings->getProxySettings();

        QString resourceUrl = QString::fromLocal8Bit(url.toEncoded());
        Connectivity::SimpleCurlRequest *request = new Connectivity::SimpleCurlRequest(resourceUrl);
        request->setProxySettings(proxySettings);

        QThread *thread = new QThread();

        request->moveToThread(thread);

        QObject::connect(thread, &QThread::started, request, &Connectivity::SimpleCurlRequest::process);
        QObject::connect(request, &Connectivity::SimpleCurlRequest::stopped, thread, &QThread::quit);

        QObject::connect(request, &Connectivity::SimpleCurlRequest::stopped, request, &Connectivity::SimpleCurlRequest::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(request, &Connectivity::SimpleCurlRequest::requestFinished, this, &FotoliaQueryEngine::requestFinishedHandler);

        thread->start();
    }

    void FotoliaQueryEngine::requestFinishedHandler(bool success) {
        LOG_INFO << "success:" << success;

        Connectivity::SimpleCurlRequest *request = qobject_cast<Connectivity::SimpleCurlRequest *>(sender());

        if (success) {
            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(request->getResponseData(), &error);

            if (error.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                QJsonValue nbResultsValue = jsonObject["nb_results"];

                if (!nbResultsValue.isUndefined()) {
                    int resultsNumber = nbResultsValue.toInt();
                    std::vector<std::shared_ptr<SuggestionArtwork> > suggestionArtworks;
                    parseResponse(jsonObject, resultsNumber, suggestionArtworks);
                    setResults(suggestionArtworks);
                    emit resultsAvailable();
                }
            } else {
                LOG_WARNING << "parsing error:" << error.errorString();
                emit errorReceived(tr("Can't parse the response"));
            }
        } else {
            LOG_WARNING << "error:" << request->getErrorString();
            emit errorReceived(request->getErrorString());
        }

        request->dispose();
    }

    void FotoliaQueryEngine::parseResponse(const QJsonObject &jsonObject, int count,
                                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
        for (int i = 0; i < count; ++i) {
            QJsonValue item = jsonObject[QString::number(i)];
            if (!item.isObject()) { continue; }
            QJsonObject object = item.toObject();

            QJsonValue url = object["thumbnail_url"];
            if (!url.isString()) { continue; }

            QJsonValue keywords = object["keywords"];
            if (!keywords.isString()) { continue; }

            QJsonValue id = object["id"];
            if (!id.isDouble()) { continue; }
            QString externalUrl = QString("https://fotolia.com/id/%1").arg((int)id.toDouble());

            QJsonValue title = object["title"];
            if (!title.isString()) { continue; }

            QStringList keywordsList = keywords.toString().split(',');

            suggestionArtworks.emplace_back(new SuggestionArtwork(url.toString(), externalUrl, title.toString(), QString(""), keywordsList));
        }
    }

    QUrl FotoliaQueryEngine::buildQuery(const QString &apiKey, const SearchQuery &query) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("search_parameters[language_id]", "2");
        urlQuery.addQueryItem("search_parameters[thumbnail_size]", "160");
        urlQuery.addQueryItem("search_parameters[limit]", QString::number(query.m_MaxResults));
        urlQuery.addQueryItem("search_parameters[order]", "nb_downloads");
        urlQuery.addQueryItem("search_parameters[words]", query.m_SearchTerms.join(' '));
        urlQuery.addQueryItem("result_columns[0]", "nb_results");
        urlQuery.addQueryItem("result_columns[1]", "title");
        urlQuery.addQueryItem("result_columns[2]", "keywords");
        urlQuery.addQueryItem("result_columns[3]", "thumbnail_url");
        urlQuery.addQueryItem("result_columns[4]", "id");
        urlQuery.addQueryItem(resultsTypeToString(query.m_Flags), "1");

        QUrl url;
        url.setUrl(QLatin1String("http://api.fotolia.com/Rest/1/search/getSearchResults"));
        url.setUserName(apiKey);
        //url.setPassword("");
        url.setQuery(urlQuery);
        return url;
    }

    QString FotoliaQueryEngine::resultsTypeToString(Common::flag_t queryFlags) const {
        if (Common::HasFlag(queryFlags, Suggestion::AllImages)) { return QLatin1String("search_parameters[filters][content_type:all]"); }
        else if (Common::HasFlag(queryFlags, Suggestion::Photos)) { return QLatin1String("search_parameters[filters][content_type:photo]"); }
        else if (Common::HasFlag(queryFlags, Suggestion::Vectors)) { return QLatin1String("search_parameters[filters][content_type:vector]"); }
        else if (Common::HasFlag(queryFlags, Suggestion::Illustrations)) { return QLatin1String("search_parameters[filters][content_type:illustration]"); }
        else { return QString(); }
    }
}
