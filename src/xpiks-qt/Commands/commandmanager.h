/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QUrl>
#include <QVector>
#include <QObject>
#include <memory>
#include <vector>
#include "../UndoRedo/ihistoryitem.h"
#include "commandbase.h"
#include "../Connectivity/analyticsuserevent.h"
#include "../Common/flags.h"
#include "icommandmanager.h"
#include "../Common/iservicebase.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../Models/artworkelement.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../KeywordsPresets/presetkeywordsmodelconfig.h"
#include "../Helpers/asynccoordinator.h"
#include "../MetadataIO/artworkssnapshot.h"
#include "../Common/flags.h"

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
    class SessionManager;
    class SwitcherModel;
}

namespace Suggestion {
    class KeywordsSuggestor;
}

namespace MetadataIO {
    class MetadataIOService;
    class MetadataIOCoordinator;
    class CsvExportModel;
}

namespace SpellCheck {
    class SpellCheckerService;
    class SpellCheckSuggestionModel;
    class DuplicatesReviewModel;
}

namespace Connectivity {
    class TelemetryService;
    class UpdateService;
    class RequestsService;
}

namespace Plugins {
    class PluginManager;
}

namespace Warnings {
    class WarningsService;
    class WarningsModel;
}

namespace QMLExtensions {
    class ColorsModel;
    class ImageCachingService;
    class VideoCachingService;
    class ArtworksUpdateHub;
}

namespace AutoComplete {
    class AutoCompleteService;
    class KeywordsAutoCompleteModel;
}

namespace Helpers {
    class HelpersQmlWrapper;
    class DatabaseManager;
}

namespace Translation {
    class TranslationService;
    class TranslationManager;
}

namespace QuickBuffer {
    class QuickBuffer;
}

namespace Maintenance {
    class MaintenanceService;
}

namespace Commands {
    class CommandManager : public QObject, public ICommandManager
    {
        Q_OBJECT
    public:
        CommandManager();
        virtual ~CommandManager();

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
        void InjectDependency(MetadataIO::MetadataIOService *metadataIOService);
        void InjectDependency(Connectivity::TelemetryService *telemetryService);
        void InjectDependency(Connectivity::UpdateService *updateService);
        void InjectDependency(Models::LogsModel *logsModel);
        void InjectDependency(MetadataIO::MetadataIOCoordinator *metadataIOCoordinator);
        void InjectDependency(Plugins::PluginManager *pluginManager);
        void InjectDependency(Models::LanguagesModel *languagesModel);
        void InjectDependency(QMLExtensions::ColorsModel *colorsModel);
        void InjectDependency(AutoComplete::AutoCompleteService *autoCompleteService);
        void InjectDependency(AutoComplete::KeywordsAutoCompleteModel *autoCompleteModel);
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
        void InjectDependency(Models::SessionManager *sessionManager);
        void InjectDependency(Warnings::WarningsModel *warningsModel);
        void InjectDependency(QuickBuffer::QuickBuffer *quickBuffer);
        void InjectDependency(Maintenance::MaintenanceService *maintenanceService);
        void InjectDependency(QMLExtensions::VideoCachingService *videoCachingService);
        void InjectDependency(Models::SwitcherModel *switcherModel);
        void InjectDependency(QMLExtensions::ArtworksUpdateHub *artworksUpdateHub);
        void InjectDependency(Connectivity::RequestsService *requestsService);
        void InjectDependency(Helpers::DatabaseManager *databaseManager);
        void InjectDependency(SpellCheck::DuplicatesReviewModel *duplicatesModel);
        void InjectDependency(MetadataIO::CsvExportModel *csvExportModel);

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
        void combineArtworks(MetadataIO::WeakArtworksSnapshot &artworks) const;
        void deleteKeywordsFromArtworks(MetadataIO::WeakArtworksSnapshot &artworks) const;
        void setArtworksForUpload(MetadataIO::ArtworksSnapshot &artworks) const;
        void setArtworksForZipping(MetadataIO::ArtworksSnapshot &artworks) const;
        void setArtworksForCsvExport(MetadataIO::ArtworksSnapshot::Container &rawSnapshot) const;
        virtual void connectArtworkSignals(Models::ArtworkMetadata *artwork) const;
        void disconnectArtworkSignals(Models::ArtworkMetadata *metadata) const;
        int readMetadata(const MetadataIO::ArtworksSnapshot &snapshot) const;
        int reimportMetadata(const MetadataIO::ArtworksSnapshot &snapshot) const;
        void writeMetadata(const MetadataIO::WeakArtworksSnapshot &artworks, bool useBackups) const;
        void addToLibrary(const MetadataIO::WeakArtworksSnapshot &artworks) const;
        void updateArtworksAtIndices(const QVector<int> &indices) const;
        void updateArtworks(const MetadataIO::WeakArtworksSnapshot &artworks) const;
        void updateArtworks(const MetadataIO::ArtworksSnapshot::Container &artworks);
        void addToRecentDirectories(const QString &path) const;
        void addToRecentFiles(const QString &path) const;
        void addToRecentFiles(const QStringList &filenames) const;
        void autoDiscoverExiftool() const;

    public:
        void generatePreviews(const MetadataIO::ArtworksSnapshot &snapshot) const;
        void submitKeywordForSpellCheck(Common::BasicKeywordsModel *item, int keywordIndex) const;
        void submitForSpellCheck(const MetadataIO::WeakArtworksSnapshot &items) const;
        void submitForSpellCheck(const std::vector<Common::BasicKeywordsModel *> &items) const;
        void submitItemForSpellCheck(Common::BasicKeywordsModel *item, Common::SpellCheckFlags flags = Common::SpellCheckFlags::All) const;
        void checkSemanticDuplicates(Common::BasicKeywordsModel *item);
        void setupSpellCheckSuggestions(Common::IMetadataOperator *item, int index, Common::SuggestionFlags flags);
        void setupSpellCheckSuggestions(std::vector<std::pair<Common::IMetadataOperator *, int> > &itemPairs, Common::SuggestionFlags flags);
        void submitForSpellCheck(const std::vector<Common::BasicKeywordsModel *> &items, const QStringList &wordsToCheck) const;
        void setupDuplicatesModel(Common::BasicMetadataModel *item);
        void setupDuplicatesModel(const std::vector<Models::ArtworkMetadata *> &items);

    public:
        void submitKeywordsForWarningsCheck(Models::ArtworkMetadata *item) const;
        void submitForWarningsCheck(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags = Common::WarningsCheckFlags::All) const;
        void submitForWarningsCheck(const MetadataIO::WeakArtworksSnapshot &items) const;

    private:
        void submitForWarningsCheck(const std::vector<Common::IBasicArtwork *> &items) const;

    public:
        void saveArtworkBackup(Models::ArtworkMetadata *metadata) const;
        void saveArtworksBackups(const MetadataIO::WeakArtworksSnapshot &artworks) const;
        void reportUserAction(Connectivity::UserAction userAction) const;
        void afterConstructionCallback();

    private:
        void afterInnerServicesInitialized();
        void executeMaintenanceJobs();
        void readSession();
        int restoreReadSession();
        int restoreFiles(const QStringList &filenames, const QStringList &vectors);

    public:
#ifdef INTEGRATION_TESTS
        int restoreSessionForTest();
#endif
        void saveSessionInBackground();

    public:
        void beforeDestructionCallback() const;
        void requestCloseApplication() const;
        void restartSpellChecking();
        void generateCompletions(const QString &prefix, Common::BasicKeywordsModel *source) const;

#ifdef INTEGRATION_TESTS
        void cleanup();
#endif

    private slots:
        void servicesInitialized(int status);

    public:
        void registerCurrentItem(Models::ArtworkMetadata *artwork);
        void registerCurrentItem(Models::ArtworkProxyBase *artworkProxy) const;
        void clearCurrentItem() const;

    public:
        // methods for getters
        Helpers::AsyncCoordinator &getInitCoordinator() { return m_InitCoordinator; }
        virtual Models::ArtworksRepository *getArtworksRepository() const { return m_ArtworksRepository; }
        virtual Models::ArtItemsModel *getArtItemsModel() const { return m_ArtItemsModel; }
        virtual Encryption::SecretsManager *getSecretsManager() const { return m_SecretsManager; }
        virtual Models::UploadInfoRepository *getUploadInfoRepository() { return m_UploadInfoRepository; }
        virtual Suggestion::KeywordsSuggestor *getKeywordsSuggestor() const { return m_KeywordsSuggestor; }
        virtual Models::SettingsModel *getSettingsModel() const { return m_SettingsModel; }
        virtual SpellCheck::SpellCheckerService *getSpellCheckerService() const { return m_SpellCheckerService; }
        virtual MetadataIO::MetadataIOService *getMetadataIOService() const { return m_MetadataIOService; }
        virtual UndoRedo::UndoRedoManager *getUndoRedoManager() const { return m_UndoRedoManager; }
        virtual QMLExtensions::ColorsModel *getColorsModel() const { return m_ColorsModel; }
        virtual Models::FilteredArtItemsProxyModel *getFilteredArtItemsModel() const { return m_FilteredItemsModel; }
        virtual Models::LogsModel *getLogsModel() const { return m_LogsModel; }
        virtual Models::ArtworkUploader *getArtworkUploader() const { return m_ArtworkUploader; }
        virtual Models::ZipArchiver *getZipArchiver() const { return m_ZipArchiver; }
        virtual Models::DeleteKeywordsViewModel *getDeleteKeywordsModel() const { return m_DeleteKeywordsViewModel; }
        virtual SpellCheck::SpellCheckSuggestionModel *getSpellSuggestionsModel() const { return m_SpellCheckSuggestionModel; }
        virtual KeywordsPresets::PresetKeywordsModel *getPresetsModel() const { return m_PresetsModel; }
        virtual Translation::TranslationService *getTranslationService() const { return m_TranslationService; }
        virtual Models::UIManager *getUIManager() const { return m_UIManager; }
        virtual QuickBuffer::QuickBuffer *getQuickBuffer() const { return m_QuickBuffer; }
        virtual Models::RecentDirectoriesModel *getRecentDirectories() const { return m_RecentDirectories; }
        virtual Models::RecentFilesModel *getRecentFiles() const { return m_RecentFiles; }
        virtual Maintenance::MaintenanceService *getMaintenanceService() const { return m_MaintenanceService; }
        virtual QMLExtensions::ImageCachingService *getImageCachingService() const { return m_ImageCachingService; }
        virtual QMLExtensions::ArtworksUpdateHub *getArtworksUpdateHub() const { return m_ArtworksUpdateHub; }
        virtual Models::ArtworkProxyModel *getArtworkProxyModel() const { return m_ArtworkProxyModel; }
        virtual Models::SessionManager *getSessionManager() const { return m_SessionManager; }
        virtual Warnings::WarningsModel *getWarningsModel() const { return m_WarningsModel; }
        virtual Models::SwitcherModel *getSwitcherModel() const { return m_SwitcherModel; }
        virtual Connectivity::RequestsService *getRequestsService() const { return m_RequestsService; }
        virtual Helpers::DatabaseManager *getDatabaseManager() const { return m_DatabaseManager; }
        virtual MetadataIO::MetadataIOCoordinator *getMetadataIOCoordinator() const { return m_MetadataIOCoordinator; }
        virtual AutoComplete::KeywordsAutoCompleteModel *getAutoCompleteModel() const { return m_AutoCompleteModel; }
        virtual QMLExtensions::VideoCachingService *getVideoCachingService() const { return m_VideoCachingService; }

#ifdef INTEGRATION_TESTS
        virtual Translation::TranslationManager *getTranslationManager() const { return m_TranslationManager; }
        virtual Models::CombinedArtworksModel *getCombinedArtworksModel() const { return m_CombinedArtworksModel; }
        virtual AutoComplete::AutoCompleteService *getAutoCompleteService() const { return m_AutoCompleteService; }
        virtual Warnings::WarningsService *getWarningsService() const { return m_WarningsService; }
        virtual Models::FindAndReplaceModel *getFindAndReplaceModel() const { return m_FindAndReplaceModel; }
        virtual MetadataIO::CsvExportModel *getCsvExportModel() const { return m_CsvExportModel; }
#endif
    private:
        Common::WordAnalysisFlags getWordAnalysisFlags() const;

    private:
        Helpers::AsyncCoordinator m_InitCoordinator;

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
        MetadataIO::MetadataIOService *m_MetadataIOService;
        Connectivity::TelemetryService *m_TelemetryService;
        Connectivity::UpdateService *m_UpdateService;
        Models::LogsModel *m_LogsModel;
        MetadataIO::MetadataIOCoordinator *m_MetadataIOCoordinator;
        Plugins::PluginManager *m_PluginManager;
        Models::LanguagesModel *m_LanguagesModel;
        QMLExtensions::ColorsModel *m_ColorsModel;
        AutoComplete::AutoCompleteService *m_AutoCompleteService;
        AutoComplete::KeywordsAutoCompleteModel *m_AutoCompleteModel;
        QMLExtensions::ImageCachingService *m_ImageCachingService;
        Models::DeleteKeywordsViewModel *m_DeleteKeywordsViewModel;
        Models::FindAndReplaceModel *m_FindAndReplaceModel;
        Helpers::HelpersQmlWrapper *m_HelpersQmlWrapper;
        KeywordsPresets::PresetKeywordsModel *m_PresetsModel;
        Translation::TranslationService *m_TranslationService;
        Translation::TranslationManager *m_TranslationManager;
        Models::UIManager *m_UIManager;
        Models::ArtworkProxyModel *m_ArtworkProxyModel;
        Models::SessionManager *m_SessionManager;
        Warnings::WarningsModel *m_WarningsModel;
        QuickBuffer::QuickBuffer *m_QuickBuffer;
        Maintenance::MaintenanceService *m_MaintenanceService;
        QMLExtensions::VideoCachingService *m_VideoCachingService;
        QMLExtensions::ArtworksUpdateHub *m_ArtworksUpdateHub;
        Models::SwitcherModel *m_SwitcherModel;
        Connectivity::RequestsService *m_RequestsService;
        Helpers::DatabaseManager *m_DatabaseManager;
        SpellCheck::DuplicatesReviewModel *m_DuplicatesModel;
        MetadataIO::CsvExportModel *m_CsvExportModel;

        QVector<Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *> m_WarningsCheckers;
        QVector<Helpers::IFileNotAvailableModel*> m_AvailabilityListeners;

        volatile bool m_ServicesInitialized;
        volatile bool m_AfterInitCalled;
        volatile int m_LastCommandID;
    };
}

#endif // COMMANDMANAGER_H
