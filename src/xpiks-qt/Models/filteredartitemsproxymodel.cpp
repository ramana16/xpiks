/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filteredartitemsproxymodel.h"
#include <QDir>
#include "artitemsmodel.h"
#include "artworkmetadata.h"
#include "artworksrepository.h"
#include "artworkelement.h"
#include "settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Commands/combinededitcommand.h"
#include "../Common/flags.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"
#include "../Models/previewartworkelement.h"
#include "../QuickBuffer/quickbuffer.h"

namespace Models {
    FilteredArtItemsProxyModel::FilteredArtItemsProxyModel(QObject *parent):
        QSortFilterProxyModel(parent),
        Common::BaseEntity(),
        m_SelectedArtworksCount(0),
        m_SortingEnabled(false) {
        // m_SortingEnabled = true;
        // this->sort(0);
        m_SearchFlags = Common::SearchFlags::AnyTermsEverything;
    }

    void FilteredArtItemsProxyModel::updateSearchFlags() {
        SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        bool searchUsingAnd = settingsModel->getSearchUsingAnd();
        bool searchByFilepath = settingsModel->getSearchByFilepath();
        // default search is not case sensitive
        m_SearchFlags = searchUsingAnd ? Common::SearchFlags::AllTermsEverything :
                                    Common::SearchFlags::AnyTermsEverything;
        Common::ApplyFlag(m_SearchFlags, searchByFilepath, Common::SearchFlags::Filepath);
    }

    void FilteredArtItemsProxyModel::setSearchTerm(const QString &value) {
        LOG_INFO << value;
        bool anyChangesNeeded = value != m_SearchTerm;

        if (anyChangesNeeded) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        updateSearchFlags();

        updateFilter();
        forceUnselectAllItems();
    }

    void FilteredArtItemsProxyModel::spellCheckAllItems() {
        LOG_DEBUG << "#";
        auto allArtworks = getAllOriginalItems();
        m_CommandManager->submitForSpellCheck(allArtworks);
        m_CommandManager->reportUserAction(Connectivity::UserAction::SpellCheck);
    }

    int FilteredArtItemsProxyModel::getOriginalIndex(int index) const {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();

        return row;
    }

    int FilteredArtItemsProxyModel::getDerivedIndex(int originalIndex) const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QModelIndex index = mapFromSource(artItemsModel->index(originalIndex, 0));
        int row = index.row();

        return row;
    }

    void FilteredArtItemsProxyModel::selectDirectory(int directoryIndex) {
        LOG_DEBUG << "directory index:" << directoryIndex;

        QVector<int> directoryItems;
        int size = this->rowCount();
        directoryItems.reserve(size);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        const ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        const QString &directory = artworksRepository->getDirectory(directoryIndex);

        LOG_DEBUG << directory;

        QDir dir(directory);
        QString directoryAbsolutePath = dir.absolutePath();

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            Q_ASSERT(metadata != NULL);

            if (metadata->isInDirectory(directoryAbsolutePath)) {
                directoryItems.append(index);
                metadata->setIsSelected(!metadata->isSelected());
            }
        }

        emit allItemsSelectedChanged();
        artItemsModel->updateItems(directoryItems, QVector<int>() << ArtItemsModel::IsSelectedRole);
    }

    void FilteredArtItemsProxyModel::combineSelectedArtworks() {
        LOG_DEBUG << "#";
        auto artworksList = getSelectedOriginalItems();
        m_CommandManager->combineArtworks(artworksList);
    }

    void FilteredArtItemsProxyModel::setSelectedItemsSaved() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->setSelectedItemsSaved(indices);
    }

    void FilteredArtItemsProxyModel::removeSelectedArtworks() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->removeSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::updateSelectedArtworks() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::updateSelectedArtworksEx(const QVector<int> &roles) {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateSelectedArtworksEx(indices, roles);
    }

    void FilteredArtItemsProxyModel::saveSelectedArtworks(bool overwriteAll, bool useBackups) {
        LOG_INFO << "ovewriteAll:" << overwriteAll << "useBackups:" << useBackups;
        // former patchSelectedArtworks
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->saveSelectedArtworks(indices, overwriteAll, useBackups);
    }

    void FilteredArtItemsProxyModel::setSelectedForUpload() {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedArtworksSnapshot();
        MetadataIO::ArtworksSnapshot snapshot(selectedArtworks);
        m_CommandManager->setArtworksForUpload(snapshot);
    }

    void FilteredArtItemsProxyModel::setSelectedForZipping() {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedArtworksSnapshot();
        MetadataIO::ArtworksSnapshot snapshot(selectedArtworks);
        m_CommandManager->setArtworksForZipping(snapshot);
    }

    bool FilteredArtItemsProxyModel::areSelectedArtworksSaved() {
        int modifiedSelectedCount = getModifiedSelectedCount();

        return modifiedSelectedCount == 0;
    }

    void FilteredArtItemsProxyModel::spellCheckSelected() {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->submitForSpellCheck(selectedArtworks);
        m_CommandManager->reportUserAction(Connectivity::UserAction::SpellCheck);
    }

    int FilteredArtItemsProxyModel::getModifiedSelectedCount(bool overwriteAll) {
        auto selectedArtworks = getSelectedOriginalItems();
        int modifiedCount = 0;

        for (ArtworkMetadata *metadata: selectedArtworks) {
            if (metadata->isModified() || overwriteAll) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void FilteredArtItemsProxyModel::removeArtworksDirectory(int index) {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();

        artItemsModel->removeArtworksDirectory(index);
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::deleteKeywordsFromSelected() {
        auto selectedItems = getSelectedOriginalItems();
        m_CommandManager->deleteKeywordsFromArtworks(selectedItems);
    }

    void FilteredArtItemsProxyModel::setSelectedForCsvExport() {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedArtworksSnapshot();
        m_CommandManager->setArtworksForCsvExport(selectedArtworks);
    }

    void FilteredArtItemsProxyModel::reimportMetadataForSelected() {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedOriginalItems();

        m_CommandManager->readMetadata(selectedArtworks);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->raiseArtworksAdded((int)selectedArtworks.size(), 0);
    }

    int FilteredArtItemsProxyModel::findSelectedItemIndex() const {
        int index = -1;

        QVector<int> indices = getSelectedIndices();
        if (indices.length() == 1) {
            index = indices.first();
        }

        return index;
    }

    void FilteredArtItemsProxyModel::removeMetadataInSelected() const {
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedArtworksSnapshot();
        Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;
        using namespace Common;
        Common::SetFlag(flags, CombinedEditFlags::EditDescription);
        Common::SetFlag(flags, CombinedEditFlags::EditKeywords);
        Common::SetFlag(flags, CombinedEditFlags::EditTitle);
        Common::SetFlag(flags, CombinedEditFlags::Clear);
        removeMetadataInItems(selectedArtworks, flags);
    }

    void FilteredArtItemsProxyModel::clearKeywords(int index) {
        LOG_INFO << "index:" << index;
        ArtItemsModel *artItemsModel = getArtItemsModel();
        int originalIndex = getOriginalIndex(index);
        ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);

        if ((metadata != NULL) && (!metadata->areKeywordsEmpty())) {
            removeKeywordsInItem(metadata);
        }
    }

    void FilteredArtItemsProxyModel::focusNextItem(int index) {
        LOG_INFO << "index:" << index;
        if (0 <= index && index < rowCount() - 1) {
            QModelIndex nextQIndex = this->index(index + 1, 0);
            QModelIndex sourceIndex = mapToSource(nextQIndex);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceIndex.row());

            if (metadata != NULL) {
                metadata->requestFocus(+1);
            }
        }
    }

    void FilteredArtItemsProxyModel::focusPreviousItem(int index) {
        LOG_INFO << "index:" << index;
        if (0 < index && index < rowCount()) {
            QModelIndex nextQIndex = this->index(index - 1, 0);
            QModelIndex sourceIndex = mapToSource(nextQIndex);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceIndex.row());

            if (metadata != NULL) {
                metadata->requestFocus(-1);
            }
        }
    }

    void FilteredArtItemsProxyModel::toggleSorted() {
        LOG_INFO << "current sorted is" << m_SortingEnabled;
        forceUnselectAllItems();

        if (!m_SortingEnabled) {
            m_SortingEnabled = true;
            sort(0);
            invalidate();
        } else {
            m_SortingEnabled = false;
            setSortRole(Qt::InitialSortOrderRole);
            sort(-1);
            invalidate();
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateAllItems();
    }

    void FilteredArtItemsProxyModel::detachVectorFromSelected() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->detachVectorsFromSelected(indices);
    }

    QObject *FilteredArtItemsProxyModel::getArtworkMetadata(int index) {
        int originalIndex = getOriginalIndex(index);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QObject *item = artItemsModel->getArtworkMetadata(originalIndex);

        return item;
    }

    QObject *FilteredArtItemsProxyModel::getBasicModel(int index) {
        int originalIndex = getOriginalIndex(index);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QObject *item = artItemsModel->getBasicModel(originalIndex);

        return item;
    }

    bool FilteredArtItemsProxyModel::hasTitleWordSpellError(int index, const QString &word) {
        bool result = false;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (metadata != NULL) {
                auto *keywordsModel = metadata->getBasicModel();
                result = keywordsModel->hasTitleWordSpellError(word);
            }
        }

        return result;
    }

    bool FilteredArtItemsProxyModel::hasDescriptionWordSpellError(int index, const QString &word) {
        bool result = false;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            LOG_INFO << originalIndex;
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (metadata != NULL) {
                auto *keywordsModel = metadata->getBasicModel();
                result = keywordsModel->hasDescriptionWordSpellError(word);
            }
        }

        return result;
    }

    void FilteredArtItemsProxyModel::registerCurrentItem(int index) const {
        LOG_INFO << index;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *artwork = artItemsModel->getArtwork(originalIndex);

            if (artwork != NULL) {
                m_CommandManager->registerCurrentItem(artwork);
            }
        }
    }

    void FilteredArtItemsProxyModel::copyToQuickBuffer(int index) const {
        LOG_INFO << index;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);

            if (metadata != NULL) {
                auto *basicModel = metadata->getBasicModel();
                auto *quickBuffer = m_CommandManager->getQuickBuffer();
                quickBuffer->setFromBasicModel(basicModel);
            }
        }
    }

    void FilteredArtItemsProxyModel::fillFromQuickBuffer(int index) const {
        LOG_INFO << index;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            artItemsModel->fillFromQuickBuffer(originalIndex);
        }
    }

    void FilteredArtItemsProxyModel::suggestCorrectionsForSelected() const {
        using namespace Common;
        flag_t flags = 0;
        Common::SetFlag(flags, SuggestionFlags::Description);
        Common::SetFlag(flags, SuggestionFlags::Title);
        Common::SetFlag(flags, SuggestionFlags::Keywords);

        auto itemsForSuggestions = getFilteredOriginalItems<std::pair<Common::IMetadataOperator *, int> >(
                    [](ArtworkMetadata *artwork) { return artwork->isSelected(); },
                    [] (ArtworkMetadata *metadata, int index, int) { return std::pair<Common::IMetadataOperator *, int>(metadata, index); });
        m_CommandManager->setupSpellCheckSuggestions(itemsForSuggestions, (SuggestionFlags)flags);
    }

    void FilteredArtItemsProxyModel::generateCompletions(const QString &prefix, int index) {
        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);

            ArtItemsModel *artItemsModel = getArtItemsModel();
            auto *basicModel = artItemsModel->getBasicModel(originalIndex);

            m_CommandManager->generateCompletions(prefix, basicModel);
        }
    }

    void FilteredArtItemsProxyModel::reviewDuplicatesInSelected() const {
        auto itemsForSuggestions = getFilteredOriginalItems<ArtworkMetadata *>(
                    [](ArtworkMetadata *artwork) { return artwork->hasDuplicates(); },
                    [] (ArtworkMetadata *artwork, int, int) { return artwork; });
        m_CommandManager->setupDuplicatesModel(itemsForSuggestions);
    }

    void FilteredArtItemsProxyModel::itemSelectedChanged(bool value) {
        int plus = value ? +1 : -1;

        m_SelectedArtworksCount += plus;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::onSelectedArtworksRemoved(int value) {
        m_SelectedArtworksCount -= value;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::onSpellCheckerAvailable(bool afterRestart) {
        LOG_INFO << "after restart:" << afterRestart;
        if (afterRestart) {
            this->spellCheckAllItems();
        }
    }

    void FilteredArtItemsProxyModel::onSettingsUpdated() {
        LOG_DEBUG << "#";
        updateSearchFlags();
        invalidateFilter();
    }

    void FilteredArtItemsProxyModel::removeMetadataInItems(MetadataIO::ArtworksSnapshot::Container &itemsToClear, Common::CombinedEditFlags flags) const {
        LOG_INFO << itemsToClear.size() << "item(s) with flags =" << (int)flags;
        std::shared_ptr<Commands::CombinedEditCommand> combinedEditCommand(new Commands::CombinedEditCommand(
                flags,
                itemsToClear));

        m_CommandManager->processCommand(combinedEditCommand);
    }

    void FilteredArtItemsProxyModel::removeKeywordsInItem(ArtworkMetadata *artwork) {
        Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;
        Common::SetFlag(flags, Common::CombinedEditFlags::EditKeywords);
        Common::SetFlag(flags, Common::CombinedEditFlags::Clear);

        MetadataIO::ArtworksSnapshot::Container items;

        items.emplace_back(new ArtworkMetadataLocker(artwork));

        removeMetadataInItems(items, flags);
    }

    void FilteredArtItemsProxyModel::setFilteredItemsSelected(bool selected) {
        LOG_INFO << selected;
        ArtItemsModel *artItemsModel = getArtItemsModel();

        QVector<int> indices;
        int size = this->rowCount();
        indices.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            if (metadata != NULL) {
                metadata->setIsSelected(selected);
                indices << index;
            }
        }

        LOG_DEBUG << "Set selected" << indices.size() << "item(s) to" << selected;
        artItemsModel->updateItems(indices, QVector<int>() << ArtItemsModel::IsSelectedRole);
        emit allItemsSelectedChanged();

        m_CommandManager->clearCurrentItem();
    }

    void FilteredArtItemsProxyModel::invertFilteredItemsSelected() {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<int> indices;
        int size = this->rowCount();
        indices.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            if (metadata != NULL) {
                metadata->invertSelection();
                indices << index;
            }
        }

        artItemsModel->updateItems(indices, QVector<int>() << ArtItemsModel::IsSelectedRole);
        emit allItemsSelectedChanged();
    }

    MetadataIO::WeakArtworksSnapshot FilteredArtItemsProxyModel::getSelectedOriginalItems() const {
        MetadataIO::WeakArtworksSnapshot items = getFilteredOriginalItems<ArtworkMetadata *>(
            [](ArtworkMetadata *metadata) { return metadata->isSelected(); },
            [] (ArtworkMetadata *metadata, int, int) { return metadata; });

        return items;
    }

    MetadataIO::ArtworksSnapshot::Container FilteredArtItemsProxyModel::getSelectedArtworksSnapshot() const {
        return getFilteredOriginalItems<std::shared_ptr<ArtworkMetadataLocker> >(
            [](ArtworkMetadata *metadata) { return metadata->isSelected(); },
            [] (ArtworkMetadata *metadata, int, int) {
            return std::shared_ptr<ArtworkMetadataLocker>(new ArtworkMetadataLocker(metadata));
    });
}

    template<typename T>
    std::vector<T> FilteredArtItemsProxyModel::getFilteredOriginalItems(std::function<bool (ArtworkMetadata *)> pred,
                                                                        std::function<T(ArtworkMetadata *, int, int)> mapper) const {
        ArtItemsModel *artItemsModel = getArtItemsModel();

        std::vector<T> filteredArtworks;
        int size = this->rowCount();

        filteredArtworks.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && pred(metadata)) {
                filteredArtworks.push_back(mapper(metadata, index, row));
            }
        }

        LOG_INFO << "Filtered" << filteredArtworks.size() << "item(s)";

        return filteredArtworks;
    }

    MetadataIO::WeakArtworksSnapshot FilteredArtItemsProxyModel::getAllOriginalItems() const {
        MetadataIO::WeakArtworksSnapshot items = getFilteredOriginalItems<ArtworkMetadata *>(
            [](ArtworkMetadata *) { return true; },
            [] (ArtworkMetadata *artwork, int, int) { return artwork; });

        return items;
    }

    QVector<int> FilteredArtItemsProxyModel::getSelectedOriginalIndices() const {
        std::vector<int> items = getFilteredOriginalItems<int>(
            [](ArtworkMetadata *artwork) { return artwork->isSelected(); },
            [] (ArtworkMetadata *, int index, int) { return index; });

        return QVector<int>::fromStdVector(items);
    }

    QVector<int> FilteredArtItemsProxyModel::getSelectedIndices() const {
        std::vector<int> items = getFilteredOriginalItems<int>(
            [](ArtworkMetadata *artwork) { return artwork->isSelected(); },
            [] (ArtworkMetadata *, int, int originalIndex) { return originalIndex; });

        return QVector<int>::fromStdVector(items);
    }

    void FilteredArtItemsProxyModel::forceUnselectAllItems() {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->forceUnselectAllItems();
        m_SelectedArtworksCount = 0;
        emit selectedArtworksCountChanged();
        emit forceUnselected();
        m_CommandManager->clearCurrentItem();
    }

    ArtItemsModel *FilteredArtItemsProxyModel::getArtItemsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        ArtItemsModel *artItemsModel = dynamic_cast<ArtItemsModel *>(sourceItemModel);

        return artItemsModel;
    }

    bool FilteredArtItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        bool hasMatch = false;

        if (metadata != NULL) {
            ArtworksRepository *repository = m_CommandManager->getArtworksRepository();
            Q_ASSERT(repository != NULL);
            qint64 directoryID = metadata->getDirectoryID();

            bool directoryIsIncluded = repository->isDirectoryIncluded(directoryID);
            if (directoryIsIncluded) {
                hasMatch = true;

                if (!m_SearchTerm.trimmed().isEmpty()) {
                    hasMatch = Helpers::hasSearchMatch(m_SearchTerm, metadata, m_SearchFlags);
                }
            }
        }

        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const {
        if (!m_SortingEnabled) {
            return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();

        ArtworkMetadata *leftMetadata = artItemsModel->getArtwork(sourceLeft.row());
        ArtworkMetadata *rightMetadata = artItemsModel->getArtwork(sourceRight.row());

        bool result = false;

        if (leftMetadata != NULL && rightMetadata != NULL) {
            const QString &leftFilepath = leftMetadata->getFilepath();
            const QString &rightFilepath = rightMetadata->getFilepath();

            QFileInfo leftFI(leftFilepath);
            QFileInfo rightFI(rightFilepath);

            QString leftFilename = leftFI.fileName();
            QString rightFilename = rightFI.fileName();

            int filenamesResult = QString::compare(leftFilename, rightFilename);

            if (filenamesResult == 0) {
                result = QString::compare(leftFilepath, rightFilepath) < 0;
            } else {
                result = filenamesResult < 0;
            }
        }

        return result;
    }

#ifdef CORE_TESTS
    int FilteredArtItemsProxyModel::retrieveNumberOfSelectedItems() {
        return getSelectedOriginalIndices().size();
    }

#endif

    MetadataIO::ArtworksSnapshot::Container FilteredArtItemsProxyModel::getSearchablePreviewOriginalItems(const QString &searchTerm,
                                                                                                          Common::SearchFlags flags) const {
        return getFilteredOriginalItems<std::shared_ptr<ArtworkMetadataLocker> >(
            [&searchTerm, flags](ArtworkMetadata *artwork) {
            return Helpers::hasSearchMatch(searchTerm, artwork, flags);
        },
            [] (ArtworkMetadata *artwork, int, int) {
            return std::shared_ptr<ArtworkMetadataLocker>(new PreviewArtworkElement(artwork)); });
    }
}
