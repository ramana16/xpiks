/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "deletekeywordsviewmodel.h"
#include <QTime>
#include "../Helpers/indiceshelper.h"
#include "artworkelement.h"
#include "../Commands/commandmanager.h"
#include "../Commands/deletekeywordscommand.h"
#include "../Common/defines.h"

namespace Models {
    DeleteKeywordsViewModel::DeleteKeywordsViewModel(QObject *parent):
        Models::ArtworksViewModel(parent),
        m_KeywordsToDeleteModel(m_HoldForDeleters),
        m_CommonKeywordsModel(m_HoldForCommon),
        m_CaseSensitive(false)
    {
    }

    void DeleteKeywordsViewModel::setArtworks(MetadataIO::WeakArtworksSnapshot &artworks) {
        LOG_DEBUG << "#";
        ArtworksViewModel::setArtworks(artworks);
        recombineKeywords();
    }

    bool DeleteKeywordsViewModel::removeUnavailableItems() {
        LOG_DEBUG << "#";
        bool anyRemoved = ArtworksViewModel::removeUnavailableItems();

        if (anyRemoved) {
            if (!isEmpty()) {
                recombineKeywords();
            }
        }

        return anyRemoved;
    }

    bool DeleteKeywordsViewModel::doRemoveSelectedArtworks() {
        bool anyRemoved = ArtworksViewModel::doRemoveSelectedArtworks();

        LOG_INFO << "Any removed:" << anyRemoved;

        if (anyRemoved) {
            if (!isEmpty()) {
                recombineKeywords();
            }
        }

        return anyRemoved;
    }

    void DeleteKeywordsViewModel::doResetModel() {
        ArtworksViewModel::doResetModel();

        m_CommonKeywordsModel.clearKeywords();
        m_KeywordsToDeleteModel.clearKeywords();

        setCaseSensitive(false);
    }

    void DeleteKeywordsViewModel::removeKeywordToDeleteAt(int keywordIndex) {
        LOG_DEBUG << "#";
        QString keyword;
        if (m_KeywordsToDeleteModel.removeKeywordAt(keywordIndex, keyword)) {
            emit keywordsToDeleteCountChanged();
        }
    }

    void DeleteKeywordsViewModel::removeLastKeywordToDelete() {
        LOG_DEBUG << "#";
        removeKeywordToDeleteAt(getKeywordsToDeleteCount() - 1);
    }

    void DeleteKeywordsViewModel::clearKeywordsToDelete() {
        LOG_DEBUG << "#";
        if (m_KeywordsToDeleteModel.clearKeywords()) {
            emit keywordsToDeleteCountChanged();
        }
    }

    QString DeleteKeywordsViewModel::removeCommonKeywordAt(int keywordIndex) {
        LOG_DEBUG << "index:" << keywordIndex;
        QString keyword;
        if (m_CommonKeywordsModel.removeKeywordAt(keywordIndex, keyword)) {
            emit commonKeywordsCountChanged();
        }

        return keyword;
    }

    void DeleteKeywordsViewModel::appendKeywordToDelete(const QString &keyword) {
        LOG_INFO << keyword;
        if (m_KeywordsToDeleteModel.appendKeyword(keyword)) {
            emit keywordsToDeleteCountChanged();

            xpiks()->submitKeywordForSpellCheck(&m_KeywordsToDeleteModel, m_KeywordsToDeleteModel.rowCount() - 1);
        }
    }

    void DeleteKeywordsViewModel::pasteKeywordsToDelete(const QStringList &keywords) {
        LOG_INFO << keywords.length() << "keyword(s)" << "|" << keywords;
        if (m_KeywordsToDeleteModel.appendKeywords(keywords) > 0) {
            emit keywordsToDeleteCountChanged();

            xpiks()->submitItemForSpellCheck(&m_KeywordsToDeleteModel, Common::SpellCheckFlags::Keywords);
        }
    }

    void DeleteKeywordsViewModel::deleteKeywords() {
        LOG_INFO << "keywords to delete:" << m_KeywordsToDeleteModel.getKeywordsCount();

        if (m_KeywordsToDeleteModel.getKeywordsCount() == 0) { return; }

        MetadataIO::ArtworksSnapshot::Container rawSnapshot(getRawSnapshot());
        auto keywordsList = m_KeywordsToDeleteModel.getKeywords();

        if (!m_CaseSensitive) {
            for (auto &keyword: keywordsList) {
                keyword = keyword.toLower();
            }
        }

        auto keywordsSet = keywordsList.toSet();

        std::shared_ptr<Commands::DeleteKeywordsCommand> deleteKeywordsCommand(
                    new Commands::DeleteKeywordsCommand(rawSnapshot, keywordsSet, m_CaseSensitive));
        m_CommandManager->processCommand(deleteKeywordsCommand);
    }

    bool DeleteKeywordsViewModel::addPreset(KeywordsPresets::ID_t presetID) {
        bool success = false;
        LOG_INFO << "preset" << presetID;
        auto *presetsModel = m_CommandManager->getPresetsModel();
        QStringList keywords;

        if (presetsModel->tryGetPreset(presetID, keywords)) {
            if (m_KeywordsToDeleteModel.appendKeywords(keywords) > 0) {
                emit keywordsToDeleteCountChanged();

                xpiks()->submitItemForSpellCheck(&m_KeywordsToDeleteModel, Common::SpellCheckFlags::Keywords);
            }
        }

        return success;
    }

    void DeleteKeywordsViewModel::recombineKeywords() {
        LOG_DEBUG << "#";
        QHash<QString, int> keywordsHash;
        fillKeywordsHash(keywordsHash);
        LOG_INFO << "Found" << keywordsHash.size() << "keyword(s)";

        QMultiMap<int, QString> selectedKeywords;

        auto hashIt = keywordsHash.constBegin();
        auto hashItEnd = keywordsHash.constEnd();

        for (; hashIt != hashItEnd; ++hashIt) {
            selectedKeywords.insert(hashIt.value(), hashIt.key());
        }

        auto it = selectedKeywords.constEnd();
        auto itBegin = selectedKeywords.constBegin();

        QStringList commonKeywords;
        commonKeywords.reserve(50);

        qsrand(QTime::currentTime().msec());
        int maxSize = 40 + qrand()%10;

        while (it != itBegin) {
            --it;

            int frequency = it.key();
            if (frequency == 0) { continue; }

            const QString &frequentKeyword = it.value();

            commonKeywords.append(frequentKeyword);
            if (commonKeywords.size() > maxSize) { break; }
        }

        LOG_INFO << "Found" << commonKeywords.size() << "common keywords";
        m_CommonKeywordsModel.setKeywords(commonKeywords);
        emit commonKeywordsCountChanged();
    }

    void DeleteKeywordsViewModel::fillKeywordsHash(QHash<QString, int> &keywordsHash) {
        LOG_DEBUG << "#";
        processArtworks([](const ArtworkElement*) { return true; },
        [&keywordsHash](int, ArtworkMetadata *metadata) {
            const auto &keywords = metadata->getKeywords();

            for (auto &keyword: keywords) {
                if (keywordsHash.contains(keyword)) {
                    keywordsHash[keyword]++;
                } else {
                    keywordsHash.insert(keyword, 1);
                }
            }
        });
    }
}
