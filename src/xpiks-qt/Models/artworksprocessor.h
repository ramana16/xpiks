/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKSPROCESSOR_H
#define ARTWORKSPROCESSOR_H

#include <QObject>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../Models/artworkmetadata.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Commands {
    class CommandManager;
}

namespace Models {
    class ArtworksProcessor : public QObject, public Common::BaseEntity, public Helpers::IFileNotAvailableModel
    {
        Q_OBJECT
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getIsError WRITE setIsError NOTIFY isErrorChanged)
        Q_PROPERTY(int percent READ getPercent NOTIFY percentChanged)
        Q_PROPERTY(int itemsCount READ getItemsCount NOTIFY itemsCountChanged)

    protected:
        enum { MAX_WORKER_THREADS = 10};

    public:
        ArtworksProcessor(QObject *parent=0) :
            QObject(parent),
            Common::BaseEntity(),
            m_ProcessedArtworksCount(0),
            m_ArtworksCount(0),
            m_ExistingMaxThreadsNumber(0),
            m_IsInProgress(false),
            m_IsError(false)
        {}

        virtual ~ArtworksProcessor() {}

    protected:
        void incProgress() { m_ProcessedArtworksCount++; updateProgress(); }

    public:
        virtual bool getInProgress() const { return m_IsInProgress; }
        void setInProgress(bool value) { m_IsInProgress = value; emit inProgressChanged(); }
        virtual int getPercent() const { return m_ArtworksCount == 0 ? 0 : (m_ProcessedArtworksCount * 100 / m_ArtworksCount); }
        void updateProgress() { emit percentChanged(); }
        bool getIsError() const { return m_IsError; }
        void setIsError(bool value) { m_IsError = value; emit isErrorChanged(); }
        virtual int getItemsCount() const { return (int)m_ArtworksSnapshot.size(); }

    public:
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE void cancelOperation() { cancelProcessing(); }

    signals:
        void inProgressChanged();
        void isErrorChanged();
        void startedProcessing();
        void percentChanged();
        void finishedProcessing();
        void itemsCountChanged();
        void itemsNumberChanged();
        void requestCloseWindow();

    public:
        void setArtworks(MetadataIO::ArtworksSnapshot &snapshot) { m_ArtworksSnapshot = std::move(snapshot); emit itemsCountChanged(); }
        void resetArtworks() { m_ArtworksSnapshot.clear(); emit itemsCountChanged(); }
        virtual bool removeUnavailableItems();

#ifdef CORE_TESTS
    public:
#else
    protected:
#endif
        const MetadataIO::ArtworksSnapshot &getArtworksSnapshot() const { return m_ArtworksSnapshot; }

    protected:
        virtual void cancelProcessing() = 0;
        virtual void innerResetModel() { /*BUMP*/ }
        void beginProcessing();
        void endProcessing();
        void endAfterFirstError();
        virtual void restrictMaxThreads();

    private:
        MetadataIO::ArtworksSnapshot m_ArtworksSnapshot;
        volatile int m_ProcessedArtworksCount;
        volatile int m_ArtworksCount;
        volatile int m_ExistingMaxThreadsNumber;
        volatile bool m_IsInProgress;
        volatile bool m_IsError;
    };
}
#endif // ARTWORKSPROCESSOR_H
