/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "currenteditableartwork.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "../Commands/commandmanager.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../Commands/expandpresetcommand.h"
#include "../Models/artworkelement.h"
#include "../Commands/deletekeywordscommand.h"
#include "../Commands/maindelegator.h"

namespace QuickBuffer {
    CurrentEditableArtwork::CurrentEditableArtwork(Models::ArtworkMetadata *artworkMetadata, size_t originalIndex, Commands::CommandManager * const commandManager):
        m_CommandManager(commandManager),
        m_OriginalIndex(originalIndex)
    {
        Q_ASSERT(commandManager != nullptr);
        Q_ASSERT(artworkMetadata != nullptr);
        m_ArtworkMetadata = artworkMetadata;

        m_ArtworkMetadata->acquire();
    }

    CurrentEditableArtwork::~CurrentEditableArtwork() {
        if (m_ArtworkMetadata->release()) {
            LOG_WARNING << "Item #" << m_ArtworkMetadata->getItemID() << "could have been removed";
        }
    }

    Common::ID_t CurrentEditableArtwork::getItemID() {
        return m_ArtworkMetadata->getItemID();
    }

    QString CurrentEditableArtwork::getTitle() {
        return m_ArtworkMetadata->getTitle();
    }

    QString CurrentEditableArtwork::getDescription() {
        return m_ArtworkMetadata->getDescription();
    }

    QStringList CurrentEditableArtwork::getKeywords() {
        return m_ArtworkMetadata->getKeywords();
    }

    void CurrentEditableArtwork::setTitle(const QString &value) {
        m_ArtworkMetadata->setTitle(value);
    }

    void CurrentEditableArtwork::setDescription(const QString &value) {
        m_ArtworkMetadata->setDescription(value);
    }

    void CurrentEditableArtwork::setKeywords(const QStringList &keywords) {
        m_ArtworkMetadata->setKeywords(keywords);
    }

    bool CurrentEditableArtwork::appendPreset(KeywordsPresets::ID_t presetID) {
        bool success = false;
        LOG_INFO << "preset" << presetID;

        std::shared_ptr<Commands::ExpandPresetCommand> expandPresetCommand(new Commands::ExpandPresetCommand(m_ArtworkMetadata, presetID));
        std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(expandPresetCommand);
        success = result->getStatus() == 0;

        return success;
    }

    bool CurrentEditableArtwork::expandPreset(int keywordIndex, KeywordsPresets::ID_t presetID) {
        bool success = false;
        LOG_INFO << "keyword" << keywordIndex << "preset" << presetID;

        std::shared_ptr<Commands::ExpandPresetCommand> expandPresetCommand(new Commands::ExpandPresetCommand(m_ArtworkMetadata, presetID, keywordIndex));
        std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(expandPresetCommand);
        success = result->getStatus() == 0;

        return success;
    }

    bool CurrentEditableArtwork::removePreset(KeywordsPresets::ID_t presetID) {
        bool success = false;
        LOG_INFO << "preset" << presetID;
        auto *presetsModel = m_CommandManager->getPresetsModel();
        QStringList keywords;

        if (presetsModel->tryGetPreset(presetID, keywords)) {
            MetadataIO::ArtworksSnapshot::Container artworksList;
            artworksList.emplace_back(new Models::ArtworkMetadataLocker(m_ArtworkMetadata));

            for (auto &keyword: keywords) {
                keyword = keyword.toLower();
            }

            std::shared_ptr<Commands::DeleteKeywordsCommand> deleteKeywordsCommand(
                        new Commands::DeleteKeywordsCommand(artworksList, keywords.toSet(), false));
            m_CommandManager->processCommand(deleteKeywordsCommand);
        }

        return success;
    }

    bool CurrentEditableArtwork::hasKeywords(const QStringList &keywordsList) {
        return m_ArtworkMetadata->hasKeywords(keywordsList);
    }

    void CurrentEditableArtwork::spellCheck() {
        xpiks()->submitItemForSpellCheck(m_ArtworkMetadata->getBasicModel());
    }

    void CurrentEditableArtwork::update() {
        xpiks()->updateArtworksAtIndices(QVector<int>() << (int)m_OriginalIndex);
    }

    Commands::MainDelegator *CurrentEditableArtwork::xpiks() {
        return m_CommandManager->getDelegator();
    }
}
