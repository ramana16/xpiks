/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QVector>
#include <QSet>
#include <QTimer>
#include <QMutex>
#include <QQmlEngine>
#include "../Common/basicmetadatamodel.h"
#include "../Common/flags.h"
#include "../Common/ibasicartwork.h"
#include "../Common/imetadataoperator.h"
#include "../Common/hold.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../UndoRedo/artworkmetadatabackup.h"

namespace MetadataIO {
    struct CachedArtwork;
    struct OriginalMetadata;
}

class QTextDocument;

namespace Models {
    class SettingsModel;

    class ArtworkMetadata:
            public QObject,
            public Common::IBasicArtwork,
            public Common::IMetadataOperator
    {
        Q_OBJECT

    public:
        ArtworkMetadata(const QString &filepath, Common::ID_t ID, qint64 directoryID);
        virtual ~ArtworkMetadata();

    private:
        enum MetadataFlags {
            FlagIsModified = 1 << 0,
            FlagsIsSelected = 1 << 1,
            FlagIsInitialized = 1 << 2, // is initialized from real file
            FlagIsAlmostInitialized = 1 << 3, // is initialized from cached storage
            FlagIsUnavailable = 1 << 4,
            FlagIsLockedForEditing = 1 << 5
        };

        inline bool getIsModifiedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsModified); }
        inline bool getIsSelectedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagsIsSelected); }
        inline bool getIsUnavailableFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsUnavailable); }
        inline bool getIsInitializedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsInitialized); }
        inline bool getIsAlmostInitializedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsAlmostInitialized); }
        inline bool getIsLockedForEditingFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsLockedForEditing); }

        inline void setIsModifiedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsModified); }
        inline void setIsSelectedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagsIsSelected); }
        inline void setIsUnavailableFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsUnavailable); }
        inline void setIsInitializedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsInitialized); }
        inline void setIsAlmostInitializedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsAlmostInitialized); }
        inline void setIsLockedForEditingFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsLockedForEditing); }

    public:
        bool initFromOrigin(const MetadataIO::OriginalMetadata &originalMetadata, bool overwrite=false);
        bool initFromStorage(const MetadataIO::CachedArtwork &cachedArtwork);
        // called when Close is pressed in the Import dialog
        void initAsEmpty(const MetadataIO::OriginalMetadata &originalMetadata);
        void initAsEmpty();

    protected:
        virtual bool initFromOriginUnsafe(const MetadataIO::OriginalMetadata &originalMetadata) { Q_UNUSED(originalMetadata); return false; }
        virtual bool initFromStorageUnsafe(const MetadataIO::CachedArtwork &cachedArtwork) { Q_UNUSED(cachedArtwork); return false; }

    public:
        virtual const QString &getFilepath() const override { return m_ArtworkFilepath; }
        virtual const QString &getThumbnailPath() const override { return m_ArtworkFilepath; }
        virtual QString getDirectory() const { QFileInfo fi(m_ArtworkFilepath); return fi.absolutePath(); }
        QString getBaseFilename() const;
        bool isInDirectory(const QString &directoryAbsolutePath) const;

        bool isModified() const { return getIsModifiedFlag(); }
        bool isSelected() const { return getIsSelectedFlag(); }
        bool isUnavailable() const { return getIsUnavailableFlag(); }
        bool isInitialized() const { return getIsInitializedFlag(); }
        bool isAlmostInitialized() const { return getIsAlmostInitializedFlag(); }
        size_t getLastKnownIndex() const { return m_LastKnownIndex; }
        virtual qint64 getFileSize() const { return m_FileSize; }
        virtual Common::ID_t getItemID() const override { return m_ID; }

    public:
        void setCurrentIndex(size_t index) { m_LastKnownIndex = index; }
        Common::WarningFlags getWarningsFlags() const { return m_WarningsFlags; }
        void setWarningsFlags(Common::WarningFlags flags) { m_WarningsFlags = flags; }
        void addWarningsFlags(Common::WarningFlags flags) { Common::SetFlag(m_WarningsFlags, flags); }
        void dropWarningsFlags(Common::WarningFlags flagsToDrop) { Common::UnsetFlag(m_WarningsFlags, flagsToDrop); }

    public:
        Common::BasicMetadataModel *getBasicModel() { return &m_MetadataModel; }
        const Common::BasicMetadataModel *getBasicModel() const { return &m_MetadataModel; }

        bool isLockedForEditing() const { return getIsLockedForEditingFlag(); }
        void setIsLockedForEditing(bool value) { setIsLockedForEditingFlag(value); }

        virtual void clearModel();
        virtual bool clearKeywords() override;
        virtual bool editKeyword(int index, const QString &replacement) override;
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags);

    public:
        virtual bool setDescription(const QString &value) override;
        virtual bool setTitle(const QString &value) override;
        virtual void setKeywords(const QStringList &keywords) override;

        bool setIsSelected(bool value);

        void invertSelection() { setIsSelected(!getIsSelectedFlag()); }

        void resetSelected() {
            if (getIsSelectedFlag()) {
                setIsSelectedFlag(false);
            }
        }

        void setFileSize(qint64 size) { m_FileSize = size; }

    public:
        bool areKeywordsEmpty() { return m_MetadataModel.areKeywordsEmpty(); }
        virtual bool removeKeywordAt(int index, QString &removed) override;
        virtual bool removeLastKeyword(QString &removed) override;
        virtual bool appendKeyword(const QString &keyword) override;
        virtual int appendKeywords(const QStringList &keywordsList) override;
        virtual bool removeKeywords(const QSet<QString> &keywordsSet, bool caseSensitive=true) override;
        virtual QString getKeywordsString() override { return m_MetadataModel.getKeywordsString(); }

    public:
        virtual Common::KeywordReplaceResult fixKeywordSpelling(int index, const QString &existing, const QString &replacement) override;
        virtual bool fixDescriptionSpelling(const QString &word, const QString &replacement) override;
        virtual bool fixTitleSpelling(const QString &word, const QString &replacement) override;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createDescriptionSuggestionsList() override;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createTitleSuggestionsList() override;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createKeywordsSuggestionsList() override;
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) override;
        virtual void afterReplaceCallback() override;
        virtual Common::BasicKeywordsModel *getBasicKeywordsModel() override;

    public:
        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

    public:
        // IBasicArtwork interface
        virtual QSet<QString> getKeywordsSet() override { return m_MetadataModel.getKeywordsSet(); }
        virtual QStringList getKeywords() override { return m_MetadataModel.getKeywords(); }
        virtual bool isEmpty() override { return m_MetadataModel.isEmpty(); }
        virtual QString getDescription() override { return m_MetadataModel.getDescription(); }
        virtual QString getTitle() override { return m_MetadataModel.getTitle(); }

   public:
        virtual qint64 getDirectoryID() const { return m_DirectoryID; }

    public:
        void markModified();
        void setUnavailable() { setIsUnavailableFlag(true); }
        void resetModified() { setIsModifiedFlag(false); }
        void requestFocus(int directionSign) { emit focusRequested(directionSign); }
        virtual void requestBackup() override;
        virtual bool expandPreset(int keywordIndex, const QStringList &presetList) override;
        virtual bool appendPreset(const QStringList &presetList) override;
        virtual bool hasKeywords(const QStringList &keywordsList) override;
        void deepDisconnect();

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        void setModified() { setIsModifiedFlag(true); }
        friend class UndoRedo::ArtworkMetadataBackup;

    signals:
        void modifiedChanged(bool newValue);
        void selectedChanged(bool newValue);
        void focusRequested(int directionSign);
        void backupRequired();
        void aboutToBeRemoved();
        void spellCheckErrorsChanged();
        void thumbnailUpdated();

    private slots:
        void backupTimerTriggered() { m_BackupTimerDelay = 0; emit backupRequired(); }

    protected:
        virtual void resetFlags() { m_MetadataFlags = 0; }

    private:
        Common::Hold m_Hold;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
        Common::BasicMetadataModel m_MetadataModel;
        QMutex m_InitMutex;
        qint64 m_FileSize;  // in bytes
        QString m_ArtworkFilepath;
        QTimer m_BackupTimer;
        int m_BackupTimerDelay;
        Common::ID_t m_ID;
        qint64 m_DirectoryID;
        volatile Common::flag_t m_MetadataFlags;
        volatile size_t m_LastKnownIndex; // optimistic guess on current index of this item in artitemsmodel
        volatile Common::WarningFlags m_WarningsFlags;
    };

    class ArtworkMetadataLocker
    {
    public:
        ArtworkMetadataLocker(Models::ArtworkMetadata *metadata):
            m_ArtworkMetadata(metadata)
        {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->acquire();
            }
        }

        virtual ~ArtworkMetadataLocker() {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->release();
            }
        }

    public:
        ArtworkMetadata *getArtworkMetadata() const { return m_ArtworkMetadata; }

    private:
        ArtworkMetadata *m_ArtworkMetadata;
    };
}

Q_DECLARE_METATYPE(Models::ArtworkMetadata *)

#endif // IMAGEMETADATA_H
