/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ISUGGESTIONQUERYENGINE_H
#define ISUGGESTIONQUERYENGINE_H

#include <QStringList>
#include <vector>
#include <memory>
#include <QObject>
#include "suggestionartwork.h"
#include "searchquery.h"

namespace Models {
    class SettingsModel;
}

namespace Suggestion {
    class SuggestionQueryEngineBase: public QObject {
        Q_OBJECT
    public:
        SuggestionQueryEngineBase(int ID, Models::SettingsModel *settingsModel = 0, QObject *parent = 0):
            QObject(parent),
            m_SettingsModel(settingsModel),
            m_EngineID(ID)
        {
        }

        virtual ~SuggestionQueryEngineBase() { }

    public:
        virtual int getMaxResults() const { return 100; }
        virtual void submitQuery(const SearchQuery &query) = 0;
        virtual QString getName() const = 0;

    public:
        int getID() const { return m_EngineID; }
        void cancelQueries() { emit cancelAllQueries(); }
        std::vector<std::shared_ptr<SuggestionArtwork> > &getLastResults() { return m_LastResults; }

        void setResults(std::vector<std::shared_ptr<SuggestionArtwork> > &results) {
            m_LastResults = std::move(results);
        }

    signals:
        void resultsAvailable();
        void cancelAllQueries();
        void errorReceived(const QString &error);

    protected:
        Models::SettingsModel *getSettingsModel() const { return m_SettingsModel; }

    private:
        std::vector<std::shared_ptr<SuggestionArtwork> > m_LastResults;
        Models::SettingsModel *m_SettingsModel;
        int m_EngineID;
    };
}

#endif // ISUGGESTIONQUERYENGINE_H
