/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKSDIRECTORIES_H
#define ARTWORKSDIRECTORIES_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QFileSystemWatcher>

#include <vector>

#include "../Common/abstractlistmodel.h"
#include "../Common/baseentity.h"
#include "../Common/flags.h"

namespace Models {
    class ArtworksRepository : public Common::AbstractListModel, public Common::BaseEntity {
        Q_OBJECT
        Q_PROPERTY(int artworksSourcesCount READ getArtworksSourcesCount NOTIFY artworksSourcesCountChanged)
    public:
        ArtworksRepository(QObject *parent = 0);
        virtual ~ArtworksRepository() {}

    public:
        enum ArtworksRepository_Roles {
            PathRole = Qt::UserRole + 1,
            UsedImagesCountRole,
            IsSelectedRole
        };

    public:
        struct RepoDir {
            RepoDir (QString absolutePath, qint64 id, int count):
                m_AbsolutePath(absolutePath), m_Id(id), m_FilesCount(count)
            { }
            RepoDir() = default;

            inline void setSelectedFlag(bool value) { Common::ApplyFlag(m_DirectoryFlags, value, Common::DirectoryFlags::IsSelected); }
            inline void setAddedAsDirectoryFlag(bool value) { Common::ApplyFlag(m_DirectoryFlags, value, Common::DirectoryFlags::IsAddedAsDirectory); }
            inline bool getSelectedFlag() const { return Common::HasFlag(m_DirectoryFlags, Common::DirectoryFlags::IsSelected); }
            inline bool getAddedAsDirectoryFlag() const { return Common::HasFlag(m_DirectoryFlags, Common::DirectoryFlags::IsAddedAsDirectory); }

            QString m_AbsolutePath = QString("");
            qint64 m_Id = 0;
            int m_FilesCount = 0;
            Common::flag_t m_DirectoryFlags;
        };

    public:
        void updateCountsForExistingDirectories();
        void cleanupEmptyDirectories(QVector<RepoDir> &directoriesToRemove,  QVector<int> &indicesToRemove);
        void resetLastUnavailableFilesCount() { m_LastUnavailableFilesCount=0; }
        void stopListeningToUnavailableFiles();
        void insertEmptyDirectory(const QString &absolutePath, size_t index, bool isSelected);

    public:
        bool beginAccountingFiles(const QStringList &items);
        void endAccountingFiles(bool filesWereAccounted);

    public:
        virtual int getNewDirectoriesCount(const QStringList &items) const;
        int getNewFilesCount(const QStringList &items) const;
        int getArtworksSourcesCount() const { return (int)m_DirectoriesList.size(); }
        bool canPurgeUnavailableFiles() const { return m_UnavailableFiles.size() == m_LastUnavailableFilesCount; }
        bool isDirectoryIncluded(qint64 directoryID) const;

    signals:
        void artworksSourcesCountChanged();
        void fileChanged(const QString &path);
        void filesUnavailable();

#ifdef CORE_TESTS
    public:
        void removeItem(int index) { removeInnerItem(index); }
    protected:
        void insertIntoUnavailable(const QString &value) { m_UnavailableFiles.insert(value); }
        const QSet<QString> &getFilesSet() const { return m_FilesSet; }
#endif

    private slots:
        void checkFileUnavailable(const QString &path);
        void onAvailabilityTimer();

    public:
        bool accountFile(const QString &filepath, qint64 &directoryID, bool isFullDirectory = false);
        void accountVector(const QString &vectorPath);
        bool removeFile(const QString &filepath, qint64 directoryID);
        void removeVector(const QString &vectorPath);
        void purgeUnavailableFiles();
        void watchFilePaths(const QStringList &filePaths);
        void unwatchFilePaths(const QStringList &filePaths);
        void updateFilesCounts();
        void updateSelectedState();

    private:
        void watchFilePath(const QString &filepath);
        qint64 generateNextID() { qint64 id = m_LastID; m_LastID++; return id; }

    public:
        const QString &getDirectory(int index) const { return m_DirectoriesList[index].m_AbsolutePath; }
        const bool getAddedAsDirectoryFlag(int index) const { return m_DirectoriesList[index].getAddedAsDirectoryFlag(); }
#ifdef CORE_TESTS
        int getFilesCountForDirectory(const QString &directory) const { size_t index; tryFindDirectory(directory, index); return m_DirectoriesList[index].m_FilesCount; }
        int getFilesCountForDirectory(int index) const { return m_DirectoriesList[index].m_FilesCount; }
#endif
        bool isFileUnavailable(const QString &filepath) const;

#ifdef INTEGRATION_TESTS
        void resetEverything();
#endif
        QSet<uint64_t> getIdsAddedAsDirectory() const;

    public:
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    public:
        Q_INVOKABLE void selectDirectory(int row);
        void selectDirectory(const QString &path);

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        virtual void removeInnerItem(int index) override {
            auto &directoryToRemove = m_DirectoriesList.at(index);
            qint64 idToRemove = directoryToRemove.m_Id;
            if (!allAreSelected())
            {
                const bool oldIsSelected = directoryToRemove.getSelectedFlag();
                const bool newIsSelected = false; // unselect folder to be deleted
                changeSelectedState(index, newIsSelected, oldIsSelected);
            }
            m_DirectoriesList.erase(m_DirectoriesList.begin() + index);
            m_DirectoryIdToIndex.remove(idToRemove);
            emit artworksSourcesCountChanged();
        }

        virtual bool checkFileExists(const QString &filename, QString &directory) const;
    public:
        bool unselectAllDirectories() { return setAllSelected(false); }
        bool selectAllDirectories() { return setAllSelected(true); }

    private:
        bool setDirectorySelected(size_t index, bool value);
        bool changeSelectedState(int row, bool newValue, bool oldValue);
        bool setAllSelected(bool value);
        size_t retrieveSelectedDirsCount() const;
        bool allAreSelected() const;
        bool tryFindDirectory(const QString &directoryPath, size_t &index) const;

    private:
        std::vector<RepoDir> m_DirectoriesList;
        QHash<qint64, size_t> m_DirectoryIdToIndex;
        QSet<QString> m_FilesSet;
        QFileSystemWatcher m_FilesWatcher;
        QTimer m_Timer;
        QSet<QString> m_UnavailableFiles;
        int m_LastUnavailableFilesCount;
        qint64 m_LastID;
    };
}

#endif // ARTWORKSDIRECTORIES_H
