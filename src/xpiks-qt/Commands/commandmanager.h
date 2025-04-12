/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QStringList>
#include <QList>
#include <QUrl>
#include <QVector>
#include <QObject>
#include <memory>
#include <vector>
#include "../UndoRedo/ihistoryitem.h"
#include "commandbase.h"
#include "../Conectivity/analyticsuserevent.h"
#include "../Common/flags.h"
#include "icommandmanager.h"
#include "../Common/iservicebase.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../Models/metadataelement.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../KeywordsPresets/presetkeywordsmodelconfig.h"

namespace Encryption {
    class SecretsManager;
}

namespace UndoRedo {
    class UndoRedoManager;
}

namespace Common {
    class BasicMetadataModel;
    class IMetadataOperator;
}

namespace Models {
    class ArtworksRepository;
    class ArtItemsModel;
    class FilteredArtItemsProxyModel;
    class CombinedArtworksModel;
    class ArtworkUploader;
    class UploadInfoRepository;
    class UploadInfo;
    class ArtworkMetadata;
    class ZipArchiver;
    class SettingsModel;
    class RecentDirectoriesModel;
    class RecentFilesModel;
    class LogsModel;
    class LanguagesModel;
    class FindAndReplaceModel;
    class DeleteKeywordsViewModel;
    class UIManager;
    class ArtworkProxyBase;
    class ArtworkProxyModel;
}

namespace Suggestion {
    class KeywordsSuggestor;
    class LocalLibrary;
}

namespace MetadataIO {
    class BackupSaverService;
    class MetadataIOCoordinator;
}

namespace SpellCheck {
    class SpellCheckerService;
    class SpellCheckSuggestionModel;
}

namespace Conectivity {
    class TelemetryService;
    class UpdateService;
}

namespace Plugins {
    class PluginManager;
}

namespace Warnings {
    class WarningsService;
}

namespace QMLExtensions {
    class ColorsModel;
    class ImageCachingService;
}

namespace AutoComplete {
    class AutoCompleteService;
}

namespace Helpers {
    class HelpersQmlWrapper;
}

namespace Translation {
    class TranslationService;
    class TranslationManager;
}

namespace QuickBuffer {
    class QuickBuffer;
}

namespace Commands {
    class CommandManager : public ICommandManager
    {
    public:
        CommandManager():
            m_ArtworksRepository(NULL),
            m_ArtItemsModel(NULL),
            m_FilteredItemsModel(NULL),
            m_CombinedArtworksModel(NULL),
            m_ArtworkUploader(NULL),
            m_UploadInfoRepository(NULL),
            m_WarningsService(NULL),
            m_SecretsManager(NULL),
            m_UndoRedoManager(NULL),
            m_ZipArchiver(NULL),
            m_KeywordsSuggestor(NULL),
            m_SettingsModel(NULL),
            m_RecentDirectories(NULL),
            m_RecentFiles(NULL),
            m_SpellCheckerService(NULL),
            m_SpellCheckSuggestionModel(NULL),
            m_MetadataSaverService(NULL),
            m_TelemetryService(NULL),
            m_UpdateService(NULL),
            m_LogsModel(NULL),
            m_LocalLibrary(NULL),
            m_MetadataIOCoordinator(NULL),
            m_PluginManager(NULL),
            m_LanguagesModel(NULL),
            m_ColorsModel(NULL),
            m_AutoCompleteService(NULL),
            m_ImageCachingService(NULL),
            m_DeleteKeywordsViewModel(NULL),
            m_FindAndReplaceModel(NULL),
            m_HelpersQmlWrapper(NULL),
            m_PresetsModel(NULL),
            m_PresetsModelConfig(NULL),
            m_TranslationService(NULL),
            m_TranslationManager(NULL),
            m_UIManager(NULL),
            m_ArtworkProxyModel(NULL),
            m_QuickBuffer(NULL),
            m_AfterInitCalled(false),
            m_LastCommandID(0)
        { }

        virtual ~CommandManager() {}

    public:
        void InjectDependency(Models::ArtworksRepository *artworkRepository);
        void InjectDependency(Models::ArtItemsModel *artItemsModel);
        void InjectDependency(Models::FilteredArtItemsProxyModel *filteredItemsModel);
        void InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel);
        void InjectDependency(Models::ArtworkUploader *artworkUploader);
        void InjectDependency(Models::UploadInfoRepository *uploadInfoRepository);
        void InjectDependency(Warnings::WarningsService *warningsService);
        void InjectDependency(Encryption::SecretsManager *secretsManager);
        void InjectDependency(UndoRedo::UndoRedoManager *undoRedoManager);
        void InjectDependency(Models::ZipArchiver *zipArchiver);
        void InjectDependency(Suggestion::KeywordsSuggestor *keywordsSuggestor);
        void InjectDependency(Models::SettingsModel *settingsModel);
        void InjectDependency(Models::RecentDirectoriesModel *recentDirectories);
        void InjectDependency(Models::RecentFilesModel *recentFiles);
        void InjectDependency(SpellCheck::SpellCheckerService *spellCheckerService);
        void InjectDependency(SpellCheck::SpellCheckSuggestionModel *spellCheckSuggestionModel);
        void InjectDependency(MetadataIO::BackupSaverService *backupSaverService);
        void InjectDependency(Conectivity::TelemetryService *telemetryService);
        void InjectDependency(Conectivity::UpdateService *updateService);
        void InjectDependency(Models::LogsModel *logsModel);
        void InjectDependency(MetadataIO::MetadataIOCoordinator *metadataIOCoordinator);
        void InjectDependency(Suggestion::LocalLibrary *localLibrary);
        void InjectDependency(Plugins::PluginManager *pluginManager);
        void InjectDependency(Models::LanguagesModel *languagesModel);
        void InjectDependency(QMLExtensions::ColorsModel *colorsModel);
        void InjectDependency(AutoComplete::AutoCompleteService *autoCompleteService);
        void InjectDependency(QMLExtensions::ImageCachingService *imageCachingService);
        void InjectDependency(Models::FindAndReplaceModel *findAndReplaceModel);
        void InjectDependency(Models::DeleteKeywordsViewModel *deleteKeywordsViewModel);
        void InjectDependency(Helpers::HelpersQmlWrapper *helpersQmlWrapper);
        void InjectDependency(KeywordsPresets::PresetKeywordsModel *presetsModel);
        void InjectDependency(KeywordsPresets::PresetKeywordsModelConfig *presetsModelConfig);
        void InjectDependency(Translation::TranslationService *translationService);
        void InjectDependency(Translation::TranslationManager *translationManager);
        void InjectDependency(Models::UIManager *uiManager);
        void InjectDependency(Models::ArtworkProxyModel *artworkProxy);
        void InjectDependency(QuickBuffer::QuickBuffer *quickBuffer);

    private:
        int generateNextCommandID() { int id = m_LastCommandID++; return id; }

    public:
        virtual std::shared_ptr<Commands::ICommandResult> processCommand(const std::shared_ptr<ICommandBase> &command) override;
        virtual void addWarningsService(Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *service) override;

    public:
        void recordHistoryItem(std::unique_ptr<UndoRedo::IHistoryItem> &historyItem) const;
        void connectEntitiesSignalsSlots() const;

    public:
        void ensureDependenciesInjected();
        void removeUnavailableFiles();

    public:
        void recodePasswords(const QString &oldMasterPassword,
                                const QString &newMasterPassword,
                                const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) const;

        void combineArtwork(Models::ArtworkMetadata *metadata, int index) const;
        void combineArtworks(std::vector<Models::MetadataElement> &artworks) const;
        void deleteKeywordsFromArtworks(std::vector<Models::MetadataElement> &artworks) const;
        void setArtworksForUpload(const QVector<Models::ArtworkMetadata*> &artworks) const;
        void setArtworksForZipping(const QVector<Models::ArtworkMetadata*> &artworks) const;
        virtual void connectArtworkSignals(Models::ArtworkMetadata *metadata) const;
        void disconnectArtworkSignals(Models::ArtworkMetadata *metadata) const;
        void readMetadata(const QVector<Models::ArtworkMetadata*> &artworks,
                          const QVector<QPair<int, int> > &rangesToUpdate) const;
        void writeMetadata(const QVector<Models::ArtworkMetadata*> &artworks, bool useBackups) const;
        void addToLibrary(const QVector<Models::ArtworkMetadata*> &artworks) const;
        void updateArtworks(const QVector<int> &indices) const;
        void updateArtworks(const QVector<QPair<int, int> > &rangesToUpdate) const;
        void addToRecentDirectories(const QString &path) const;
        void addToRecentFiles(const QString &path) const;
        void addToRecentFiles(const QStringList &filenames) const;
        void autoDiscoverExiftool() const;
#ifdef QT_DEBUG
    private:
        void openInitialFiles();
    public:
        void addInitialArtworks(const QList<QUrl> &filePaths);
#endif

    public:
        void generatePreviews(const QVector<Models::ArtworkMetadata*> &items) const;
        void submitKeywordForSpellCheck(Common::BasicKeywordsModel *item, int keywordIndex) const;
        void submitForSpellCheck(const QVector<Models::ArtworkMetadata*> &items) const;
        void submitForSpellCheck(const QVector<Common::BasicKeywordsModel *> &items) const;
        void submitItemForSpellCheck(Common::BasicKeywordsModel *item, Common::SpellCheckFlags flags = Common::SpellCheckFlags::All) const;
        void setupSpellCheckSuggestions(Common::IMetadataOperator *item, int index, Common::SuggestionFlags flags);
        void setupSpellCheckSuggestions(std::vector<std::pair<Common::IMetadataOperator *, int> > &itemPairs, Common::SuggestionFlags flags);
        void submitForSpellCheck(const QVector<Common::BasicKeywordsModel *> &items, const QStringList &wordsToCheck) const;

    public:
        void submitKeywordsForWarningsCheck(Models::ArtworkMetadata *item) const;
        void submitForWarningsCheck(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags = Common::WarningsCheckFlags::All) const;
        void submitForWarningsCheck(const QVector<Models::ArtworkMetadata*> &items) const;

    private:
        void submitForWarningsCheck(const QVector<Common::IBasicArtwork *> &items) const;

    public:
        void saveArtworkBackup(Models::ArtworkMetadata *metadata) const;
        void saveArtworksBackups(const QVector<Models::ArtworkMetadata *> &artworks) const;
        void reportUserAction(Conectivity::UserAction userAction) const;
        void cleanupLocalLibraryAsync() const;
        void afterConstructionCallback();
        void beforeDestructionCallback() const;
        void requestCloseApplication() const;
        void restartSpellChecking();
#ifndef CORE_TESTS
        void autoCompleteKeyword(const QString &keyword, QObject *notifyObject) const;
#endif

#ifdef INTEGRATION_TESTS
        void cleanup();
#endif

    public:
        void registerCurrentItem(const Models::MetadataElement &metadataElement);
        void registerCurrentItem(Models::ArtworkProxyBase *artworkProxy) const;
        void clearCurrentItem() const;

    public:
        // methods for getters
        virtual Models::ArtworksRepository *getArtworksRepository() const { return m_ArtworksRepository; }
        virtual Models::ArtItemsModel *getArtItemsModel() const { return m_ArtItemsModel; }
        virtual Encryption::SecretsManager *getSecretsManager() const { return m_SecretsManager; }
        virtual Models::UploadInfoRepository *getUploadInfoRepository() { return m_UploadInfoRepository; }
        virtual Suggestion::KeywordsSuggestor *getKeywordsSuggestor() const { return m_KeywordsSuggestor; }
        virtual Models::SettingsModel *getSettingsModel() const { return m_SettingsModel; }
        virtual SpellCheck::SpellCheckerService *getSpellCheckerService() const { return m_SpellCheckerService; }
        virtual MetadataIO::BackupSaverService *getBackupSaverService() const { return m_MetadataSaverService; }
        virtual UndoRedo::UndoRedoManager *getUndoRedoManager() const { return m_UndoRedoManager; }
        virtual QMLExtensions::ColorsModel *getColorsModel() const { return m_ColorsModel; }
        virtual Models::FilteredArtItemsProxyModel *getFilteredArtItemsModel() const { return m_FilteredItemsModel; }
        virtual Models::LogsModel *getLogsModel() const { return m_LogsModel; }
        virtual Models::ArtworkUploader *getArtworkUploader() const { return m_ArtworkUploader; }
        virtual Models::ZipArchiver *getZipArchiver() const { return m_ZipArchiver; }
        virtual Models::DeleteKeywordsViewModel *getDeleteKeywordsModel() const { return m_DeleteKeywordsViewModel; }
        virtual SpellCheck::SpellCheckSuggestionModel *getSpellSuggestionsModel() const { return m_SpellCheckSuggestionModel; }
        virtual KeywordsPresets::PresetKeywordsModel *getPresetsModel() const { return m_PresetsModel; }
        virtual KeywordsPresets::PresetKeywordsModelConfig *getPresetsModelConfig() const { return m_PresetsModelConfig; }
        virtual Translation::TranslationService *getTranslationService() const { return m_TranslationService; }
        virtual Models::UIManager *getUIManager() const { return m_UIManager; }
        virtual QuickBuffer::QuickBuffer *getQuickBuffer() const { return m_QuickBuffer; }
        virtual Models::RecentDirectoriesModel *getRecentDirectories() const { return m_RecentDirectories; }
        virtual Models::RecentFilesModel *getRecentFiles() const { return m_RecentFiles; }

#ifdef INTEGRATION_TESTS
        virtual Translation::TranslationManager *getTranslationManager() const { return m_TranslationManager; }
        virtual MetadataIO::MetadataIOCoordinator *getMetadataIOCoordinator() const { return m_MetadataIOCoordinator; }
        virtual Models::CombinedArtworksModel *getCombinedArtworksModel() const { return m_CombinedArtworksModel; }
        virtual AutoComplete::AutoCompleteService *getAutoCompleteService() const { return m_AutoCompleteService; }
        virtual Warnings::WarningsService *getWarningsService() const { return m_WarningsService; }
        virtual Models::FindAndReplaceModel *getFindAndReplaceModel() const { return m_FindAndReplaceModel; }
#endif

    private:
        Models::ArtworksRepository *m_ArtworksRepository;
        Models::ArtItemsModel *m_ArtItemsModel;
        Models::FilteredArtItemsProxyModel *m_FilteredItemsModel;
        Models::CombinedArtworksModel *m_CombinedArtworksModel;
        Models::ArtworkUploader *m_ArtworkUploader;
        Models::UploadInfoRepository *m_UploadInfoRepository;
        Warnings::WarningsService *m_WarningsService;
        Encryption::SecretsManager *m_SecretsManager;
        UndoRedo::UndoRedoManager *m_UndoRedoManager;
        Models::ZipArchiver *m_ZipArchiver;
        Suggestion::KeywordsSuggestor *m_KeywordsSuggestor;
        Models::SettingsModel *m_SettingsModel;
        Models::RecentDirectoriesModel *m_RecentDirectories;
        Models::RecentFilesModel *m_RecentFiles;
        SpellCheck::SpellCheckerService *m_SpellCheckerService;
        SpellCheck::SpellCheckSuggestionModel *m_SpellCheckSuggestionModel;
        MetadataIO::BackupSaverService *m_MetadataSaverService;
        Conectivity::TelemetryService *m_TelemetryService;
        Conectivity::UpdateService *m_UpdateService;
        Models::LogsModel *m_LogsModel;
        Suggestion::LocalLibrary *m_LocalLibrary;
        MetadataIO::MetadataIOCoordinator *m_MetadataIOCoordinator;
        Plugins::PluginManager *m_PluginManager;
        Models::LanguagesModel *m_LanguagesModel;
        QMLExtensions::ColorsModel *m_ColorsModel;
        AutoComplete::AutoCompleteService *m_AutoCompleteService;
        QMLExtensions::ImageCachingService *m_ImageCachingService;
        Models::DeleteKeywordsViewModel *m_DeleteKeywordsViewModel;
        Models::FindAndReplaceModel *m_FindAndReplaceModel;
        Helpers::HelpersQmlWrapper *m_HelpersQmlWrapper;
        KeywordsPresets::PresetKeywordsModel *m_PresetsModel;
        KeywordsPresets::PresetKeywordsModelConfig *m_PresetsModelConfig;
        Translation::TranslationService *m_TranslationService;
        Translation::TranslationManager *m_TranslationManager;
        Models::UIManager *m_UIManager;
        Models::ArtworkProxyModel *m_ArtworkProxyModel;
        QuickBuffer::QuickBuffer *m_QuickBuffer;

        QVector<Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *> m_WarningsCheckers;
        QVector<Helpers::IFileNotAvailableModel*> m_AvailabilityListeners;
#ifdef QT_DEBUG
        QList<QUrl> m_InitialFilesToOpen;
#endif

        volatile bool m_AfterInitCalled;
        volatile int m_LastCommandID;
    };
}

#endif // COMMANDMANAGER_H
