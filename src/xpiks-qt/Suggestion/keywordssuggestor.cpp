/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QHash>
#include <QString>
#include <QMultiMap>
#include "keywordssuggestor.h"
#include "suggestionartwork.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "../QuickBuffer/quickbuffer.h"
#include "suggestionqueryenginebase.h"
#include "shutterstockqueryengine.h"
#include "locallibraryqueryengine.h"
#include "fotoliaqueryengine.h"
#include "gettyqueryengine.h"
#include "../Models/switchermodel.h"
#include "../Models/settingsmodel.h"

#define LINEAR_TIMER_INTERVAL 1000

namespace Suggestion {
    KeywordsSuggestor::KeywordsSuggestor(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        m_SuggestedKeywords(m_HoldPlaceholder, this),
        m_AllOtherKeywords(m_HoldPlaceholder, this),
        m_SelectedArtworksCount(0),
        m_SelectedSourceIndex(0),
        m_LocalSearchIndex(-1),
        m_IsInProgress(false)
    {
        setLastErrorString(tr("No results found"));
        qsrand(QTime::currentTime().msec());

        m_LinearTimer.setSingleShot(true);
        QObject::connect(&m_LinearTimer, &QTimer::timeout, this, &KeywordsSuggestor::onLinearTimer);
    }

    KeywordsSuggestor::~KeywordsSuggestor() {
        qDeleteAll(m_QueryEngines);
    }

    void KeywordsSuggestor::setExistingKeywords(const QSet<QString> &keywords) {
        LOG_DEBUG << "#";
        m_ExistingKeywords.clear(); m_ExistingKeywords.unite(keywords);
    }

    void KeywordsSuggestor::initSuggestionEngines() {
        Q_ASSERT(m_CommandManager != NULL);
        auto *settingsModel = m_CommandManager->getSettingsModel();
        auto *metadataIOService = m_CommandManager->getMetadataIOService();

        int id = 0;
        m_QueryEngines.append(new ShutterstockQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new GettyQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new FotoliaQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new LocalLibraryQueryEngine(id++, metadataIOService));
        m_LocalSearchIndex = m_QueryEngines.length() - 1;

        int length = m_QueryEngines.length();
        for (int i = 0; i < length; ++i) {
            SuggestionQueryEngineBase *engine = m_QueryEngines.at(i);
            m_QueryEnginesNames.append(engine->getName());

            QObject::connect(engine, &SuggestionQueryEngineBase::resultsAvailable,
                             this, &KeywordsSuggestor::resultsAvailableHandler);

            QObject::connect(engine, &SuggestionQueryEngineBase::errorReceived,
                             this, &KeywordsSuggestor::errorsReceivedHandler);
        }
    }

    void KeywordsSuggestor::setSuggestedArtworks(std::vector<std::shared_ptr<SuggestionArtwork> > &suggestedArtworks) {
        LOG_INFO << suggestedArtworks.size() << "item(s)";

        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearKeywords();
        m_AllOtherKeywords.clearKeywords();

        Models::SwitcherModel *switcher = m_CommandManager->getSwitcherModel();
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        const bool sequentialLoading =
                (switcher->getProgressiveSuggestionPreviewsOn() ||
                 settingsModel->getUseProgressiveSuggestionPreviews()) &&
                (!getIsLocalSearch());
        LOG_INFO << "With sequential loading:" << sequentialLoading;

        if (sequentialLoading) {
            const int increment = settingsModel->getProgressiveSuggestionIncrement();
            m_LoadedPreviewsNumber = increment;
            LOG_DEBUG << "Progressive increment is" << increment;
        } else {
            m_LoadedPreviewsNumber = (int)suggestedArtworks.size();
        }

        beginResetModel();
        {
            m_Suggestions = std::move(suggestedArtworks);
        }
        endResetModel();

        if (sequentialLoading) {
            m_LinearTimer.start(LINEAR_TIMER_INTERVAL);
        }

        unsetInProgress();
        emit suggestionArrived();
        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
        emit selectedArtworksCountChanged();
    }

    void KeywordsSuggestor::clear() {
        LOG_DEBUG << "#";

        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearKeywords();
        m_AllOtherKeywords.clearKeywords();
        m_ExistingKeywords.clear();
        m_LoadedPreviewsNumber = 0;

        beginResetModel();
        {
            m_Suggestions.clear();
        }
        endResetModel();

        unsetInProgress();
        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
        emit selectedArtworksCountChanged();
    }

    void KeywordsSuggestor::setSelectedSourceIndex(int value) {
        if (!m_IsInProgress && (value != m_SelectedSourceIndex)) {
            if (0 <= value &&
                    value < m_QueryEngines.length()) {
                LOG_INFO << "Selected query source index:" << value;
                m_SelectedSourceIndex = value;
                emit selectedSourceIndexChanged();
                emit isLocalSearchChanged();
            }
        }
    }

    bool KeywordsSuggestor::getIsLocalSearch() const {
        bool result = m_SelectedSourceIndex == m_LocalSearchIndex;
        Q_ASSERT(result == isLocalSuggestionActive());
        return result;
    }

    void KeywordsSuggestor::resultsAvailableHandler() {
        Q_ASSERT(0 <= m_SelectedSourceIndex && m_SelectedSourceIndex < m_QueryEngines.length());
        SuggestionQueryEngineBase *currentEngine = m_QueryEngines.at(m_SelectedSourceIndex);
        SuggestionQueryEngineBase *senderEngine = qobject_cast<SuggestionQueryEngineBase *>(sender());

        if ((senderEngine == nullptr) || (senderEngine->getID() != currentEngine->getID())) {
            qInfo() << "Received results from another engine:" <<
                       ((senderEngine != nullptr) ? senderEngine->getID() : (-1)) << "current:" << currentEngine->getID();
        }

        unsetInProgress();
        auto &results = currentEngine->getLastResults();
        setSuggestedArtworks(results);
    }

    void KeywordsSuggestor::errorsReceivedHandler(const QString &error) {
        unsetInProgress();
        setLastErrorString(error);
    }

    void KeywordsSuggestor::onLinearTimer() {
        const int size = rowCount();
        if (m_LoadedPreviewsNumber >= size) { return; }

        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        const int increment = settingsModel->getProgressiveSuggestionIncrement();

        QModelIndex firstIndex = this->index(m_LoadedPreviewsNumber);
        int nextIndex = m_LoadedPreviewsNumber + increment;
        if (nextIndex > size/2) {
            nextIndex = size - 1;
        }

        QModelIndex lastIndex = this->index(nextIndex);
        m_LoadedPreviewsNumber = nextIndex;

        emit dataChanged(firstIndex, lastIndex, QVector<int>() << UrlRole);
        m_LinearTimer.start(LINEAR_TIMER_INTERVAL);
    }

    void KeywordsSuggestor::onLanguageChanged() {
        m_QueryEnginesNames.clear();
        int length = m_QueryEngines.length();

        for (int i = 0; i < length; ++i) {
            SuggestionQueryEngineBase *engine = m_QueryEngines.at(i);
            m_QueryEnginesNames.append(engine->getName());
        }

        setLastErrorString(tr("No results found"));
    }

    bool KeywordsSuggestor::isLocalSuggestionActive() const {
        const int index = getSelectedSourceIndex();
        if ((index < 0) || (index >= (int)m_QueryEngines.size())) { return false; }

        SuggestionQueryEngineBase *engine = m_QueryEngines.at(index);
        LocalLibraryQueryEngine *localEngine = dynamic_cast<LocalLibraryQueryEngine*>(engine);
        bool result = localEngine != nullptr;
        return result;
    }

    QString KeywordsSuggestor::removeSuggestedKeywordAt(int keywordIndex) {
        LOG_INFO << "Index:" << keywordIndex;

        QString keyword;
        if (m_SuggestedKeywords.removeKeywordAt(keywordIndex, keyword)) {
            emit suggestedKeywordsCountChanged();
            LOG_INFO << "Removed:" << keyword;
        }

        return keyword;
    }

    QString KeywordsSuggestor::removeOtherKeywordAt(int keywordIndex) {
        LOG_INFO << "Index:" << keywordIndex;

        QString keyword;
        if (m_AllOtherKeywords.removeKeywordAt(keywordIndex, keyword)) {
            emit otherKeywordsCountChanged();
            LOG_INFO << "Removed:" << keyword;
        }
        return keyword;
    }

    void KeywordsSuggestor::setArtworkSelected(int index, bool newState) {
        if (index < 0 || (size_t)index >= m_Suggestions.size()) {
            return;
        }

        auto &suggestionArtwork = m_Suggestions.at(index);
        suggestionArtwork->setIsSelected(newState);

        int sign = newState ? +1 : -1;
        accountKeywords(suggestionArtwork->getKeywordsSet(), sign);
        m_SelectedArtworksCount += sign;
        emit selectedArtworksCountChanged();

        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        updateSuggestedKeywords();
    }

    void KeywordsSuggestor::copyToQuickBuffer(int index) const {
        if (index < 0 || (size_t)index >= m_Suggestions.size()) {
            LOG_WARNING << "Index is out of bounds: " << index;
            return;
        }

        auto &suggestionArtwork = m_Suggestions.at(index);
        auto *quickBuffer = m_CommandManager->getQuickBuffer();
        quickBuffer->setFromSuggestionArtwork(suggestionArtwork);
    }

    void KeywordsSuggestor::searchArtworks(const QString &searchTerm, int resultsType) {
        LOG_INFO << "[" << searchTerm << "], search type:" << resultsType;

        if (!m_IsInProgress && !searchTerm.trimmed().isEmpty()) {
            setInProgress();

            SuggestionQueryEngineBase *engine = m_QueryEngines.at(m_SelectedSourceIndex);
            SearchQuery query(searchTerm, resultsType);
            engine->submitQuery(query);

            if (dynamic_cast<LocalLibraryQueryEngine*>(engine) == NULL) {
                m_CommandManager->reportUserAction(Connectivity::UserAction::SuggestionRemote);
            } else {
                m_CommandManager->reportUserAction(Connectivity::UserAction::SuggestionLocal);
            }
        }
    }

    void KeywordsSuggestor::cancelSearch() {
        LOG_DEBUG << "#";
        SuggestionQueryEngineBase *engine = m_QueryEngines.at(m_SelectedSourceIndex);
        engine->cancelQueries();
    }

    void KeywordsSuggestor::clearSuggested() {
        LOG_DEBUG << "#";

        while (!m_SuggestedKeywords.isEmpty()) {
            QString keyword = removeSuggestedKeywordAt(0);
            appendKeywordToOther(keyword);
        }
    }

    void KeywordsSuggestor::resetSelection() {
        LOG_DEBUG << "#";
        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearKeywords();
        m_AllOtherKeywords.clearKeywords();
        m_ExistingKeywords.clear();

        beginResetModel();
        {
            for (auto &item: m_Suggestions) {
                item->setIsSelected(false);
            }
        }
        endResetModel();

        unsetInProgress();
        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
        emit selectedArtworksCountChanged();
    }

    int KeywordsSuggestor::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_Suggestions.size();
    }

    QVariant KeywordsSuggestor::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || (size_t)row >= m_Suggestions.size()) { return QVariant(); }

        auto &suggestionArtwork = m_Suggestions.at(row);

        switch (role) {
        case UrlRole: {
#ifdef QT_DEBUG
            if (row <= m_LoadedPreviewsNumber) {
                return suggestionArtwork->getUrl();
            } else {
                return QVariant();
            }
#else
            return suggestionArtwork->getUrl();
#endif
        }
        case IsSelectedRole:
            return suggestionArtwork->getIsSelected();
        case ExternalUrlRole:
            return suggestionArtwork->getExternalUrl();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> KeywordsSuggestor::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[UrlRole] = "url";
        roles[IsSelectedRole] = "isselected";
        roles[ExternalUrlRole] = "externalurl";
        return roles;
    }

    void KeywordsSuggestor::accountKeywords(const QSet<QString> &keywords, int sign) {
        foreach(const QString &keyword, keywords) {
            if (m_KeywordsHash.contains(keyword)) {
                m_KeywordsHash[keyword] += sign;
            } else {
                m_KeywordsHash.insert(keyword, 1);
            }
        }
    }

    QSet<QString> KeywordsSuggestor::getSelectedArtworksKeywords() const {
        QSet<QString> allKeywords;
        size_t size = m_Suggestions.size();

        for (size_t i = 0; i < size; ++i) {
            auto &artwork = m_Suggestions.at(i);

            if (artwork->getIsSelected()) {
                const QSet<QString> &currentKeywords = artwork->getKeywordsSet();
                allKeywords.unite(currentKeywords);
            }
        }

        return allKeywords;
    }

    void KeywordsSuggestor::updateSuggestedKeywords() {
        QStringList suggestedKeywords, otherKeywords;
        QMultiMap<int, QString> selectedKeywords;
        int lowerThreshold, upperThreshold;
        calculateBounds(lowerThreshold, upperThreshold);

        QHash<QString, int>::const_iterator hashIt = m_KeywordsHash.constBegin();
        QHash<QString, int>::const_iterator hashItEnd = m_KeywordsHash.constEnd();

        for (; hashIt != hashItEnd; ++hashIt) {
            selectedKeywords.insert(hashIt.value(), hashIt.key());
        }

        QMultiMap<int, QString>::const_iterator it = selectedKeywords.constEnd();
        QMultiMap<int, QString>::const_iterator itBegin = selectedKeywords.constBegin();

        int maxSuggested = 35 + (qrand() % 10);
        int maxUpperBound = 40 + (qrand() % 5);
        int maxOthers = 35 + (qrand() % 10);

        suggestedKeywords.reserve(maxSuggested);
        otherKeywords.reserve(maxOthers);

        bool canAddToSuggested, canAddToOthers;
        const bool isOnlyOneArtwork = (m_SelectedArtworksCount == 1);

        while (it != itBegin) {
            --it;

            int frequency = it.key();
            const QString &frequentKeyword = it.value();

            if (frequency == 0) { continue; }
            if (m_ExistingKeywords.contains(frequentKeyword.toLower())) {
                LOG_DEBUG << "Skipping existing keyword" << frequentKeyword;
                continue;
            }

            int suggestedCount = suggestedKeywords.length();

            canAddToSuggested = (frequency >= upperThreshold) && (suggestedCount <= maxUpperBound);
            canAddToOthers = frequency >= lowerThreshold;

            if (isOnlyOneArtwork || canAddToSuggested ||
                    (canAddToOthers && (suggestedCount <= maxSuggested))) {
                suggestedKeywords.append(frequentKeyword);
            } else if (canAddToOthers || (otherKeywords.length() <= maxOthers)) {
                otherKeywords.append(frequentKeyword);

                if (otherKeywords.length() > maxOthers) {
                    break;
                }
            }
        }

        m_SuggestedKeywords.setKeywords(suggestedKeywords);
        m_AllOtherKeywords.setKeywords(otherKeywords);

        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
    }

    void KeywordsSuggestor::calculateBounds(int &lowerBound, int &upperBound) const {
        if (m_SelectedArtworksCount <= 2) {
            lowerBound = 1;
            upperBound = qMax(m_SelectedArtworksCount, 1);
        } else if (m_SelectedArtworksCount <= 4) {
            lowerBound = 2;
            upperBound = 3;
        } else if (m_SelectedArtworksCount <= 5) {
            lowerBound = 2;
            upperBound = 3;
        } else if (m_SelectedArtworksCount <= 9) {
            upperBound = m_SelectedArtworksCount / 2;
            lowerBound = upperBound - 1;
        } else if (m_SelectedArtworksCount <= 15) {
            upperBound = m_SelectedArtworksCount / 2 - 1;
            lowerBound = upperBound - 2;
        } else {
            upperBound = m_SelectedArtworksCount / 2;
            lowerBound = upperBound - 2;
        }
    }
}
