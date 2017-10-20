/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QUrl>
#include <QSize>
#include <QHash>
#include <QQuickTextDocument>
#include <deque>
#include <vector>
#include <memory>
#include "../Common/abstractlistmodel.h"
#include "../Common/baseentity.h"
#include "../Common/ibasicartwork.h"
#include "../Common/iartworkssource.h"
#include "../Helpers/ifilenotavailablemodel.h"

namespace Common {
    class BasicMetadataModel;
}

namespace QMLExtensions {
    class ArtworkUpdateRequest;
}

namespace Models {
    class ArtworkMetadata;
    class ArtworkElement;

    class ArtItemsModel:
        public Common::AbstractListModel,
        public Common::BaseEntity,
        public Common::IArtworksSource,
        public Helpers::IFileNotAvailableModel
    {
    Q_OBJECT
    Q_PROPERTY(int modifiedArtworksCount READ getModifiedArtworksCount NOTIFY modifiedArtworksCountChanged)

    public:
        ArtItemsModel(QObject *parent=0);

        virtual ~ArtItemsModel();

    public:
        enum ArtItemsModel_Roles {
            ArtworkDescriptionRole = Qt::UserRole + 1,
            EditArtworkDescriptionRole,
            ArtworkFilenameRole,
            ArtworkTitleRole,
            EditArtworkTitleRole,
            KeywordsStringRole,
            KeywordsCountRole,
            IsModifiedRole,
            IsSelectedRole,
            EditIsSelectedRole,
            HasVectorAttachedRole,
            BaseFilenameRole,
            IsVideoRole,
            ArtworkThumbnailRole,
            RolesNumber
        };

    public:
        virtual ArtworkMetadata *createMetadata(const QString &filepath, qint64 directoryID);
        void deleteAllItems();
#ifdef INTEGRATION_TESTS
        void fakeDeleteAllItems();
#endif

    public:
        int getModifiedArtworksCount();

        void updateModifiedCount() { emit modifiedArtworksCountChanged(); }
        void updateItems(const QVector<int> &indices, const QVector<int> &roles);
        void forceUnselectAllItems() const;
        virtual bool removeUnavailableItems() override;
        void generateAboutToBeRemoved();
        int getMinChangedItemsCountForReset() const { return getRangesLengthForReset(); }

    public:
        Q_INVOKABLE void updateAllItems();
        /*Q_INVOKABLE*/ void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);
        Q_INVOKABLE void pasteKeywords(int metadataIndex, const QStringList &keywords);
        Q_INVOKABLE void addSuggestedKeywords(int metadataIndex, const QStringList &keywords);
        Q_INVOKABLE void suggestCorrections(int metadataIndex);
        Q_INVOKABLE void backupItem(int metadataIndex);

        Q_INVOKABLE void combineArtwork(int index) { doCombineArtwork(index); }
        Q_INVOKABLE int dropFiles(const QList<QUrl> &urls);

        /*Q_INVOKABLE*/ void setSelectedItemsSaved(const QVector<int> &selectedIndices);

        /*Q_INVOKABLE*/ void removeSelectedArtworks(QVector<int> &selectedIndices);

        /*Q_INVOKABLE*/ void updateSelectedArtworks(const QVector<int> &selectedIndices);
        /*Q_INVOKABLE*/ void updateSelectedArtworksEx(const QVector<int> &selectedIndices, const QVector<int> roles);

        /*Q_INVOKABLE*/ void saveSelectedArtworks(const QVector<int> &selectedIndices, bool overwriteAll, bool useBackups);

        /*Q_INVOKABLE*/ ArtworkMetadata *getArtworkMetadata(int index) const;

        /*Q_INVOKABLE*/ Common::BasicMetadataModel *getBasicModel(int index) const;

        Q_INVOKABLE QSize retrieveImageSize(int metadataIndex) const;
        Q_INVOKABLE QString retrieveFileSize(int metadataIndex) const;
        Q_INVOKABLE QString getArtworkFilepath(int index) const;
        Q_INVOKABLE QString getAttachedVectorPath(int metadataIndex) const;
        Q_INVOKABLE QString getArtworkDateTaken(int metadataIndex) const;

        Q_INVOKABLE int addRecentDirectory(const QString &directory);
        Q_INVOKABLE int addRecentFile(const QString &file);
        Q_INVOKABLE void initDescriptionHighlighting(int metadataIndex, QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(int metadataIndex, QQuickTextDocument *document);

        Q_INVOKABLE void editKeyword(int metadataIndex, int keywordIndex, const QString &replacement);
        Q_INVOKABLE void plainTextEdit(int metadataIndex, const QString &rawKeywords, bool spaceIsSeparator=false);

        /*Q_INVOKABLE*/ void detachVectorsFromSelected(const QVector<int> &selectedIndices);

        Q_INVOKABLE void expandPreset(int artworkIndex, int keywordIndex, int presetIndex);
        Q_INVOKABLE void expandLastAsPreset(int metadataIndex);
        Q_INVOKABLE void addPreset(int metadataIndex, int presetIndex);
        Q_INVOKABLE bool acceptCompletionAsPreset(int metadataIndex, int completionID);
        Q_INVOKABLE void initSuggestion(int metadataIndex);
        Q_INVOKABLE void setupDuplicatesModel(int metadataIndex);

        void fillFromQuickBuffer(int metadataIndex);

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    public slots:
        int addLocalArtworks(const QList<QUrl> &artworksPaths);
        int addLocalDirectories(const QList<QUrl> &directories);

        void itemModifiedChanged(bool) { updateModifiedCount(); }
        void spellCheckErrorsChanged();
        void onFilesUnavailableHandler();
        void onArtworkBackupRequested();
        void onArtworkEditingPaused();
        void onUndoStackEmpty();
        void userDictUpdateHandler(const QStringList &keywords, bool overwritten);
        void userDictClearedHandler();

    public:
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) override;
        void beginAccountingFiles(int filesCount);
        void beginAccountingFiles(int start, int end);
        void endAccountingFiles();
        void beginAccountingManyFiles();
        void endAccountingManyFiles();

    public:
        void syncArtworksIndices();
        void insertArtwork(int index, ArtworkMetadata *artwork);
        void appendArtwork(ArtworkMetadata *artwork);
        void removeArtworks(const QVector<QPair<int, int> > &ranges);
        ArtworkMetadata *getArtwork(size_t index) const;
        void raiseArtworksAdded(int imagesCount, int vectorsCount);
        void raiseArtworksChanged(bool navigateToCurrent);
        virtual void updateItemsAtIndices(const QVector<int> &indices);
        void updateItemsAtIndicesEx(const QVector<int> &indices, const QVector<int> &roles);
        virtual void updateItemsInRanges(const QVector<QPair<int, int> > &ranges);
        void updateItemsInRangesEx(const QVector<QPair<int, int> > &ranges, const QVector<int> &roles);
        void setAllItemsSelected(bool selected);
        int attachVectors(const QHash<QString, QHash<QString, QString> > &vectorsPaths, QVector<int> &indicesToUpdate) const;

    public:
        // update hub related
        void processUpdateRequests(const std::vector<std::shared_ptr<QMLExtensions::ArtworkUpdateRequest> > &updateRequests);
        void updateArtworks(const QSet<qint64> &artworkIDs, const QVector<int> &rolesToUpdate);

    public:
        // IARTWORKSSOURCE
        virtual Common::IBasicArtwork *getBasicArtwork(int index) const override;

        virtual int getArtworksCount() const override { return (int)m_ArtworkList.size(); }

    private:
        void updateItemAtIndex(int metadataIndex);
        int addDirectories(const QStringList &directories);
        void doAddDirectory(const QString &directory, QStringList &filesList);
        int addFiles(const QStringList &filepath);

    private:
        void doCombineArtwork(int index);
        Models::ArtworkMetadata *accessArtwork(size_t index) const;

    signals:
        void modifiedArtworksCountChanged();
        void artworksChanged(bool needToMoveCurrentItem);
        void artworksAdded(int imagesCount, int vectorsCount);
        void selectedArtworksRemoved(int count);
        void fileWithIndexUnavailable(size_t index);
        void unavailableArtworksFound();
        void unavailableVectorsFound();
        void userDictUpdate(const QString &word);

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        virtual int getRangesLengthForReset() const override;
        virtual void removeInnerItem(int row) override;
        virtual void removeInnerItemRange(int start, int end) override;

    private:
        void destroyInnerItem(ArtworkMetadata *artwork);
        void doRemoveItemsAtIndices(QVector<int> &indicesToRemove);
        void doRemoveItemsInRanges(const QVector<QPair<int, int> > &rangesToRemove);
        void getSelectedItemsIndices(QVector<int> &indices);

    public:
        QVector<int> getArtworkStandardRoles() const;

    private:
        void fillStandardRoles(QVector<int> &roles) const;

#ifdef CORE_TESTS
    public:
        const std::deque<ArtworkMetadata *> &getFinalizationList() const { return m_FinalizationList; }
#endif

#ifdef INTEGRATION_TESTS
    public:
        ArtworkMetadata *findArtworkByFilepath(const QString &filepath);
#endif

    public:
        const std::deque<ArtworkMetadata *> &getArtworkList() const { return m_ArtworkList; }

    private:
        std::deque<ArtworkMetadata *> m_ArtworkList;
        std::deque<ArtworkMetadata *> m_FinalizationList;
#ifdef QT_DEBUG
        std::deque<ArtworkMetadata *> m_DestroyedList;
#endif
        qint64 m_LastID;
    };
}

#endif // ARTSITEMSMODEL_H
