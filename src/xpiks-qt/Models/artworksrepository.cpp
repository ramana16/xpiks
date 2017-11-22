/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "artworksrepository.h"
#include <QDir>
#include <QSet>
#include <QFileInfo>
#include <QRegExp>
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"

namespace Models {
    ArtworksRepository::ArtworksRepository(QObject *parent) :
        AbstractListModel(parent),
        m_LastUnavailableFilesCount(0),
        m_LastID(0)
    {
        QObject::connect(&m_FilesWatcher, &QFileSystemWatcher::fileChanged,
                     this, &ArtworksRepository::checkFileUnavailable);

        m_Timer.setInterval(4000); //4 sec
        m_Timer.setSingleShot(true); //single shot
        QObject::connect(&m_Timer, &QTimer::timeout, this, &ArtworksRepository::onAvailabilityTimer);
    }

    int ArtworksRepository::isEmpty(int index) const {
        bool empty = false;
        if ((0 <= index) && (index < (int)m_DirectoriesList.size())) {
            empty = (m_DirectoriesList[index].m_FilesCount == 0);
        } else {
            empty = true;
        }
        return empty;
    }

    void ArtworksRepository::refresh() {
        emit dataChanged(index(0), index(rowCount() - 1), QVector<int>());
        emit refreshRequired();
    }

    void ArtworksRepository::stopListeningToUnavailableFiles() {
        LOG_DEBUG << "#";

        QStringList files = m_FilesWatcher.files();
        if (!files.isEmpty()) {
            m_FilesWatcher.removePaths(files);
        }

        QStringList directories = m_FilesWatcher.directories();
        if (!directories.isEmpty()) {
            m_FilesWatcher.removePaths(directories);
        }
    }

    bool ArtworksRepository::beginAccountingFiles(const QStringList &items) {
        int count = getNewDirectoriesCount(items);
        bool shouldAccountFiles = count > 0;
        if (shouldAccountFiles) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + count - 1);
        }

        return shouldAccountFiles;
    }

    void ArtworksRepository::endAccountingFiles(bool filesWereAccounted) {
        if (filesWereAccounted) {
            endInsertRows();
        }

        emit artworksSourcesCountChanged();
    }

    /*virtual */
    int ArtworksRepository::getNewDirectoriesCount(const QStringList &items) const {
        QSet<QString> filteredFiles;

        foreach (const QString &filepath, items) {
            if (!m_FilesSet.contains(filepath)) {
                filteredFiles.insert(filepath);
            }
        }

        QSet<QString> filteredDirectories;

        foreach (const QString &filepath, filteredFiles) {
            QString directory;
            if (checkFileExists(filepath, directory)) {
                filteredDirectories.insert(directory);
            }
        }

        int count = 0;
        size_t phony;
        foreach (const QString &directory, filteredDirectories) {
            if (!tryFindDirectory(directory, phony)) {
                count++;
            }
        }

        return count;
    }

    int ArtworksRepository::getNewFilesCount(const QStringList &items) const {
        int count = 0;
        QSet<QString> itemsSet = QSet<QString>::fromList(items);

        foreach (const QString &filepath, itemsSet) {
            if (!m_FilesSet.contains(filepath)) {
                count++;
            }
        }

        return count;
    }

    bool ArtworksRepository::isDirectorySelected(qint64 directoryID) const {
        bool isSelected = false;

        size_t index = 0;
        if (tryFindDirectoryByID(directoryID, index)) {
            isSelected = m_DirectoriesList[index].getIsSelectedFlag();
        }

        return isSelected;
    }

    void ArtworksRepository::onUndoStackEmpty() {
        LOG_DEBUG << "#";
        cleanupEmptyDirectories();
    }

    bool ArtworksRepository::accountFile(const QString &filepath, qint64 &directoryID, Common::flag_t directoryFlags) {
        bool wasModified = false;
        QString absolutePath;

        if (this->checkFileExists(filepath, absolutePath) &&
                !m_FilesSet.contains(filepath)) {

            int occurances = 0;
            size_t index;
            bool alreadyExists = tryFindDirectory(absolutePath, index);
            if (!alreadyExists) {
                qint64 id = generateNextID();
                LOG_INFO << "Adding new directory" << absolutePath << "with index" << m_DirectoriesList.size() << "and id" << id;
                m_DirectoriesList.emplace_back(absolutePath, id, 0);
                auto &item = m_DirectoriesList.back();
                item.setIsSelectedFlag(true);
                index = m_DirectoriesList.size() - 1;
                directoryID = id;
#ifdef CORE_TESTS
                if (m_CommandManager != nullptr)
#endif
                {
                    m_CommandManager->addToRecentDirectories(absolutePath);
                }
            } else {
                auto &item = m_DirectoriesList[index];
                occurances = item.m_FilesCount;
                directoryID = item.m_ID;
            }

            // watchFilePath(filepath);
            m_FilesSet.insert(filepath);
            auto &item = m_DirectoriesList[index];
            item.setIsRemovedFlag(false);
            item.m_FilesCount = occurances + 1;
            if (Common::HasFlag(directoryFlags, Common::DirectoryFlags::IsAddedAsDirectory)) { item.setAddedAsDirectoryFlag(true); }
            if (Common::HasFlag(directoryFlags, Common::DirectoryFlags::IsSelected)) { item.setIsSelectedFlag(true); }
            wasModified = true;
        }

        return wasModified;
    }

    void ArtworksRepository::accountVector(const QString &vectorPath) {
        watchFilePath(vectorPath);
    }

    bool ArtworksRepository::removeFile(const QString &filepath, qint64 directoryID) {
        bool result = false;

        if (m_FilesSet.contains(filepath)) {
            size_t existingIndex = 0;
            if (tryFindDirectoryByID(directoryID, existingIndex)) {
                auto &item = m_DirectoriesList[existingIndex];
                item.m_FilesCount--;
                Q_ASSERT(item.m_FilesCount >= 0);
                if (item.m_FilesCount == 0) { item.setIsRemovedFlag(true); }

                m_FilesWatcher.removePath(filepath);
                m_FilesSet.remove(filepath);

                result = true;
            } else {
                Q_ASSERT(false);
            }
        }

        return result;
    }

    void ArtworksRepository::removeVector(const QString &vectorPath) {
        m_FilesWatcher.removePath(vectorPath);
    }

    void ArtworksRepository::cleanupEmptyDirectories() {
        LOG_DEBUG << "#";
        size_t count = m_DirectoriesList.size();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve((int)count);

        for (size_t i = 0; i < count; ++i) {
            auto &directory = m_DirectoriesList[i];
            if (directory.m_FilesCount == 0) {
                indicesToRemove.append((int)i);
            }
        }

        if (!indicesToRemove.isEmpty()) {
            LOG_INFO << indicesToRemove.length() << "empty directory(ies)...";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);

            updateSelectedState();
        }
    }

    void ArtworksRepository::purgeUnavailableFiles() {
        LOG_DEBUG << "#";
        m_UnavailableFiles.clear();
        m_LastUnavailableFilesCount = 0;
    }

    void ArtworksRepository::watchFilePaths(const QStringList &filePaths) {
#ifndef CORE_TESTS
        if (!filePaths.empty()) {
            m_FilesWatcher.addPaths(filePaths);
        }
#else
        Q_UNUSED(filePaths);
#endif
    }

    void ArtworksRepository::unwatchFilePaths(const QStringList &filePaths) {
#ifndef CORE_TESTS
        if (!filePaths.empty()) {
            m_FilesWatcher.removePaths(filePaths);
        }
#else
        Q_UNUSED(filePaths);
#endif
    }

    void ArtworksRepository::updateFilesCounts() {
        if (m_DirectoriesList.empty()) { return; }

        auto first = this->index(0);
        auto last = this->index(rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << UsedImagesCountRole);

        emit refreshRequired();
    }

    void ArtworksRepository::updateSelectedState() {
        if (m_DirectoriesList.empty()) { return; }

        auto first = this->index(0);
        auto last = this->index(rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << IsSelectedRole);
    }

    void ArtworksRepository::watchFilePath(const QString &filepath) {
#ifndef CORE_TESTS
        m_FilesWatcher.addPath(filepath);
#else
        Q_UNUSED(filepath);
#endif
    }

    bool ArtworksRepository::tryGetDirectoryPath(qint64 directoryID, QString &absolutePath) const {
        bool found = false;

        size_t index;
        if (tryFindDirectoryByID(directoryID, index)) {
            absolutePath = m_DirectoriesList[index].m_AbsolutePath;
            found = true;
        }

        return found;
    }

    bool ArtworksRepository::isFileUnavailable(const QString &filepath) const {
        bool isUnavailable = false;

        if (m_UnavailableFiles.contains(filepath)) {
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
            {
#else
            if(!QFileInfo(filepath).exists()) {
#endif

                isUnavailable = true;
            }
        }

        return isUnavailable;
    }

    QStringList ArtworksRepository::retrieveFullDirectories() const {
        QStringList directoriesList;

        for (auto &dir: m_DirectoriesList) {
            if (dir.getAddedAsDirectoryFlag()) {
                directoriesList.push_back(dir.m_AbsolutePath);
            }
        }

        return directoriesList;
    }

    void ArtworksRepository::restoreFullDirectories(const QStringList &directories) {
        LOG_DEBUG << directories.size() << "directory(ies)";

        bool anyChanged = false;

        for (auto &dir: directories) {
            size_t index = 0;
            if (tryFindDirectory(dir, index)) {
                LOG_DEBUG << dir << "marked as full";
                m_DirectoriesList[index].setAddedAsDirectoryFlag(true);
                anyChanged = true;
            }
        }

#ifdef QT_DEBUG
        if (anyChanged) {
            emit dataChanged(this->index(0), this->index(this->rowCount() - 1), QVector<int>() << IsFullRole);
        }
#endif
    }

#ifdef INTEGRATION_TESTS
    void ArtworksRepository::resetEverything() {
        m_DirectoriesList.clear();
        m_FilesSet.clear();
    }
#endif

    int ArtworksRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_DirectoriesList.size();
    }

    QVariant ArtworksRepository::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= (int)m_DirectoriesList.size())) {
            return QVariant();
        }

        auto &directory = m_DirectoriesList.at(index.row());

        switch (role) {
        case PathRole: {
            QDir dir(directory.m_AbsolutePath);
            return dir.dirName();
        }
        case UsedImagesCountRole:
            return directory.m_FilesCount;
        case IsSelectedRole:
            return !allAreSelected() && directory.getIsSelectedFlag();
        case IsRemovedRole:
            return directory.getIsRemovedFlag();
        case IsFullRole:
            return directory.getAddedAsDirectoryFlag();
        default:
            return QVariant();
        }
    }

    QSet<qint64> ArtworksRepository::consolidateSelectionForEmpty() {
        LOG_DEBUG << "#";
        QSet<qint64> result;

        bool anyChange = false;

        if (!allAreSelected()) {
            const bool newIsSelected = false; // unselect folder to be deleted
            const size_t size = m_DirectoriesList.size();

            for (size_t i = 0; i < size; i++) {
                auto &directory = m_DirectoriesList[i];
                if (!directory.isValid()) {
                    const bool oldIsSelected = directory.getIsSelectedFlag();
                    if (oldIsSelected) {
                        result.insert(directory.m_ID);
                    }

                    if (changeSelectedState(i, newIsSelected, oldIsSelected)) {
                        anyChange = true;
                    }
                }
            }
        }

        if (anyChange) {
#ifndef CORE_TESTS
            LOG_DEBUG << "Updating artworks";
            auto *filteredArtItemsModel = m_CommandManager->getFilteredArtItemsModel();
            Q_ASSERT(filteredArtItemsModel != NULL);
            filteredArtItemsModel->updateFilter();
#endif
        }

        return result;
    }

    void ArtworksRepository::restoreDirectoriesSelection(const QSet<qint64> &selectedDirectories) {
        LOG_DEBUG << "#";
        if (selectedDirectories.empty()) { return; }

        const size_t size = m_DirectoriesList.size();

        for (size_t i = 0; i < size; i++) {
            auto &directory = m_DirectoriesList[i];
            if (selectedDirectories.contains(directory.m_ID)) {
                directory.setIsSelectedFlag(true);
            }
        }
    }

    void ArtworksRepository::toggleDirectorySelected(size_t row) {
        LOG_INFO << row;
        if (row >= m_DirectoriesList.size()) { return; }
        Q_ASSERT(m_DirectoriesList[row].isValid());

        auto &directory = m_DirectoriesList.at(row);

        const bool oldValue = directory.getIsSelectedFlag();
        const bool newValue = !oldValue;
        LOG_DEBUG << row << "old" << oldValue << "new" << newValue;

        if (changeSelectedState(row, newValue, oldValue)) {
            updateSelectedState();
#ifndef CORE_TESTS
            LOG_DEBUG << "Updating artworks";
            auto *filteredArtItemsModel = m_CommandManager->getFilteredArtItemsModel();
            Q_ASSERT(filteredArtItemsModel != NULL);
            filteredArtItemsModel->updateFilter();
#endif
        }
    }

    bool ArtworksRepository::setDirectorySelected(size_t index, bool value) {
        auto &directory = m_DirectoriesList[index];
        //if (!directory.isValid()) { return false; }

        bool changed = directory.getIsSelectedFlag() != value;
        directory.setIsSelectedFlag(value);

        return changed;
    }

    bool ArtworksRepository::changeSelectedState(size_t row, bool newValue, bool oldValue) {
        Q_ASSERT(row < m_DirectoriesList.size());
        if (oldValue == newValue) { return false; }

        bool anySelectionChanged = false;

        const size_t count = m_DirectoriesList.size();
        if (count == 1) {
            Q_ASSERT(row == 0);
            anySelectionChanged = setDirectorySelected(row, true);
            return anySelectionChanged;
        }

        const size_t selectedCount = retrieveSelectedDirsCount();
        const bool allAreSelected = selectedCount == count;
        const bool wasSelected = (oldValue == true);
        const bool onlyOneSelected = (selectedCount == 1);
        const bool isUnselected = (newValue == false);

        if (allAreSelected && (count > 1)) {
            Q_ASSERT(newValue == false);
            unselectAllDirectories();
            setDirectorySelected(row, true);
            anySelectionChanged = true;
        } else if (onlyOneSelected && wasSelected && isUnselected) {
            selectAllDirectories();
            anySelectionChanged = true;
        } else {
            anySelectionChanged = setDirectorySelected(row, newValue);
        }

        return anySelectionChanged;
    }

    bool ArtworksRepository::setAllSelected(bool value) {
        bool anyChange = false;
        size_t size = m_DirectoriesList.size();

        for (size_t i = 0; i < size; i++) {
            if (setDirectorySelected(i, value)) {
                anyChange = true;
            }
        }

        if (anyChange) {
            updateSelectedState();
        }

        return anyChange;
    }

    size_t ArtworksRepository::retrieveSelectedDirsCount() const {
        size_t count = 0;

        for (auto &directory: m_DirectoriesList) {
            if (directory.getIsSelectedFlag()) {
                count++;
            }
        }

        return count;
    }

    bool ArtworksRepository::allAreSelected() const {
        bool anyUnselected = false;

        for (auto &item: m_DirectoriesList) {
            if (!item.getIsSelectedFlag()) {
                anyUnselected = true;
                break;
            }
        }

        return !anyUnselected;
    }

    bool ArtworksRepository::tryFindDirectory(const QString &directoryPath, size_t &index) const {
        bool found = false;
        const size_t size = m_DirectoriesList.size();

        for (size_t i = 0; i < size; ++i) {
            auto &item = m_DirectoriesList[i];
            if (QString::compare(item.m_AbsolutePath, directoryPath, Qt::CaseInsensitive) == 0) {
                found = true;
                index = i;
                break;
            }
        }

        return found;
    }

    bool ArtworksRepository::tryFindDirectoryByID(qint64 id, size_t &index) const {
        bool found = false;
        const size_t size = m_DirectoriesList.size();

        for (size_t i = 0; i < size; ++i) {
            auto &item = m_DirectoriesList[i];
            if (item.m_ID == id) {
                found = true;
                index = i;
                break;
            }
        }

        return found;
    }

    QHash<int, QByteArray> ArtworksRepository::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[UsedImagesCountRole] = "usedimagescount";
        roles[IsSelectedRole] = "isselected";
        roles[IsRemovedRole] = "isremoved";
        roles[IsFullRole] = "isfull";
        return roles;
    }

    void ArtworksRepository::removeInnerItem(int index) {
        auto &directoryToRemove = m_DirectoriesList.at(index);
        if (!allAreSelected()) {
            const bool oldIsSelected = directoryToRemove.getIsSelectedFlag();
            const bool newIsSelected = false; // unselect folder to be deleted
            changeSelectedState(index, newIsSelected, oldIsSelected);
        }
        m_DirectoriesList.erase(m_DirectoriesList.begin() + index);
    }

    /*virtual */
    bool ArtworksRepository::checkFileExists(const QString &filename, QString &directory) const {
        bool exists = false;
        QFileInfo fi(filename);

#ifndef CORE_TESTS
        exists = fi.exists();
#else
        exists = true;
#endif

        if (exists) {
            directory = fi.absolutePath();
        }

        return exists;
    }

    void ArtworksRepository::checkFileUnavailable(const QString &path) {
        LOG_INFO << "File changed:" << path;

        QFileInfo fi(path);
        if (!fi.exists()) {
            LOG_INFO << "File become unavailable:" << path;
            m_UnavailableFiles.insert(fi.absoluteFilePath());
            LOG_DEBUG << "Starting availability timer...";
            m_Timer.start();
        }
    }

    void ArtworksRepository::onAvailabilityTimer() {
        int currentUnavailableSize = m_UnavailableFiles.size();
        LOG_INFO << "Current:" << currentUnavailableSize << "Last:" << m_LastUnavailableFilesCount;

        if (currentUnavailableSize > m_LastUnavailableFilesCount) {
            m_LastUnavailableFilesCount = currentUnavailableSize;
            emit filesUnavailable();
        }
    }

    FilteredArtworksRepository::FilteredArtworksRepository(ArtworksRepository *artworksRepository) {
        setArtworksRepository(artworksRepository);
    }

    void FilteredArtworksRepository::setArtworksRepository(ArtworksRepository *artworksRepository) {
        Q_ASSERT(artworksRepository != nullptr);
        setSourceModel(artworksRepository);

        QObject::connect(artworksRepository, &ArtworksRepository::artworksSourcesCountChanged,
                         this, &FilteredArtworksRepository::artworksSourcesCountChanged);
        QObject::connect(artworksRepository, &ArtworksRepository::refreshRequired,
                         this, &FilteredArtworksRepository::onRefreshRequired);
    }

    int FilteredArtworksRepository::getOriginalIndex(int index) {
        LOG_INFO << index;
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    void FilteredArtworksRepository::selectDirectory(int row) {
        LOG_INFO << row;
        int originalRow = getOriginalIndex(row);
        ArtworksRepository *artworksRepository = getArtworksRepository();
        artworksRepository->toggleDirectorySelected(originalRow);
    }

    bool FilteredArtworksRepository::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

#ifndef QT_DEBUG
        ArtworksRepository *artworksRepository = getArtworksRepository();
        bool isEmpty = artworksRepository->isEmpty(sourceRow);
        return !isEmpty;
#else
        Q_UNUSED(sourceRow);
        return true;
#endif
    }

    void FilteredArtworksRepository::onRefreshRequired() {
        LOG_DEBUG << "#";
        this->invalidate();
    }

    ArtworksRepository *FilteredArtworksRepository::getArtworksRepository() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        ArtworksRepository *artworksRepository = dynamic_cast<ArtworksRepository *>(sourceItemModel);
        Q_ASSERT(artworksRepository != nullptr);
        return artworksRepository;
    }

}


