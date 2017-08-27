/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gettyqueryengine.h"
#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../Encryption/aes-qt.h"
#include "../Connectivity/simplecurlrequest.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"
#include "suggestionartwork.h"

namespace Suggestion {
    GettyQueryEngine::GettyQueryEngine(int engineID, Models::SettingsModel *settingsModel):
        SuggestionQueryEngineBase(engineID, settingsModel)
    {
        m_GettyImagesAPIKey = QLatin1String("17a45639c3bf88f7a6d549759af398090c3f420e53a61a06d7a2a2b153c89fc9470b2365dae8c6d92203287dc6f69f55b230835a8fb2a70b24e806771b750690");
    }

    void GettyQueryEngine::submitQuery(const SearchQuery &query) {
        LOG_INFO << query.m_SearchTerms;
        QUrl url = buildQuery(query);

        QString decodedAPIKey = Encryption::decodeText(m_GettyImagesAPIKey, "MasterPassword");

        auto *settings = getSettingsModel();
        auto *proxySettings = settings->retrieveProxySettings();

        QString resourceUrl = QString::fromLocal8Bit(url.toEncoded());
        Connectivity::SimpleCurlRequest *request = new Connectivity::SimpleCurlRequest(resourceUrl);
        request->setRawHeaders(QStringList() << "Api-Key: " + decodedAPIKey);
        request->setProxySettings(proxySettings);

        QThread *thread = new QThread();

        request->moveToThread(thread);

        QObject::connect(thread, &QThread::started, request, &Connectivity::SimpleCurlRequest::process);
        QObject::connect(request, &Connectivity::SimpleCurlRequest::stopped, thread, &QThread::quit);

        QObject::connect(request, &Connectivity::SimpleCurlRequest::stopped, request, &Connectivity::SimpleCurlRequest::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(request, &Connectivity::SimpleCurlRequest::requestFinished, this, &GettyQueryEngine::requestFinishedHandler);

        thread->start();
    }

    void GettyQueryEngine::requestFinishedHandler(bool success) {
        LOG_INFO << "success:" << success;

        Connectivity::SimpleCurlRequest *request = qobject_cast<Connectivity::SimpleCurlRequest *>(sender());

        if (success) {
            QJsonParseError error;

            QJsonDocument document = QJsonDocument::fromJson(request->getResponseData(), &error);
            if (error.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                QJsonValue nbResultsValue = jsonObject["result_count"];

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

    void GettyQueryEngine::parseResponse(const QJsonObject &jsonObject, int count,
                                         std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
        Q_UNUSED(count);
        if (!jsonObject.contains("images")) { return; }
        QJsonValue imagesValue = jsonObject["images"];

        if (imagesValue.isArray()) {
            QJsonArray imagesArray = imagesValue.toArray();
            int size = imagesArray.size();
            for (int i = 0; i < size; ++i) {
                QJsonValue element = imagesArray.at(i);
                if (!element.isObject()) { continue; }

                QJsonObject item = element.toObject();

                QString url;
                QStringList keywords;

                if (item.contains("display_sizes")) {
                    parseUrl(item["display_sizes"], url);
                }

                if (item.contains("keywords")) {
                    parseKeywords(item["keywords"], keywords);
                }

                QString title;
                QString description;

                if (item.contains("title")) {
                    QJsonValue value = item["title"];
                    if (value.isString()) {
                         title = value.toString();
                    }
                }

                if (item.contains("caption")) {
                    QJsonValue value = item["caption"];
                    if (value.isString()) {
                         description = value.toString();
                    }
                }

                // TODO: parse external url from istock
                suggestionArtworks.emplace_back(new SuggestionArtwork(url, title, description, keywords, false));
            }
        }
    }

    void GettyQueryEngine::parseUrl(const QJsonValue &previewObject, QString &url) {
        if (!previewObject.isArray()) { return; }

        QJsonArray previews = previewObject.toArray();
        int size = previews.size();

        if (size > 0) {
            QJsonValue element = previews.first();
            if (element.isObject()) {
                QJsonObject firstItem = element.toObject();
                if (firstItem.contains("uri") && firstItem.contains("is_watermarked")) {
                    QJsonValue value = firstItem["uri"];
                    if (value.isString()) {
                        url = value.toString();
                    }
                }
            }
        }
    }

    void GettyQueryEngine::parseKeywords(const QJsonValue &keywordsObject, QStringList &keywords) {
        if (keywordsObject.isArray()) {
            QJsonArray keywordsArray = keywordsObject.toArray();

            int size = keywordsArray.size();
            keywords.reserve(size);

            for (int i = 0; i < size; ++i) {
                QJsonValue element = keywordsArray.at(i);
                if (!element.isObject()) { continue; }

                QJsonObject keywordItemObject = element.toObject();
                if (keywordItemObject.contains("text")) {
                    QJsonValue textValue = keywordItemObject["text"];
                    if (textValue.isString()) {
                        keywords.append(textValue.toString());
                    }
                }
            }
        }
    }

    QUrl GettyQueryEngine::buildQuery(const SearchQuery &query) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("fields", "keywords,preview,title,id,caption");
        urlQuery.addQueryItem("phrase", query.m_SearchTerms.join(' '));
        urlQuery.addQueryItem("page", "1");
        urlQuery.addQueryItem("page_size", "100");
        urlQuery.addQueryItem("sort_order", "most_popular");

        if (!Common::HasFlag(query.m_Flags, Suggestion::AllImages)) {
            urlQuery.addQueryItem("graphical_styles", resultsTypeToString(query.m_Flags));
        }

        QUrl url;
        url.setUrl(QLatin1String("https://api.gettyimages.com:443/v3/search/images"));
        url.setQuery(urlQuery);
        return url;
    }

    QString GettyQueryEngine::resultsTypeToString(Common::flag_t queryFlags) const {
        if (Common::HasFlag(queryFlags, Suggestion::Photos)) { return QLatin1String("photography"); }
        else if (Common::HasFlag(queryFlags, Suggestion::Vectors)) { return QLatin1String("illustration"); }
        else if (Common::HasFlag(queryFlags, Suggestion::Illustrations)) { return QLatin1String("fine_art"); }
        else { return QString(); }
    }
}
