/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKUPLOADER_H
#define ARTWORKUPLOADER_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFutureWatcher>
#include "../Connectivity/testconnection.h"
#include "../AutoComplete/stringsautocompletemodel.h"
#include "../AutoComplete/stocksftplistmodel.h"
#include "../Connectivity/uploadwatcher.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Helpers {
    class TestConnectionResult;
}

namespace Connectivity {
    class IFtpCoordinator;
}

namespace Commands {
    class CommandManager;
}

namespace Helpers {
    class AsyncCoordinator;
}

namespace Models {
#define PERCENT_EPSILON 0.0001
    class ArtworkMetadata;

    class ArtworkUploader:
            public QObject,
            public Common::BaseEntity,
            public Helpers::IFileNotAvailableModel
    {
        Q_PROPERTY(int percent READ getUIPercent NOTIFY percentChanged)
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getHasErrors WRITE setHasErrors NOTIFY hasErrorsChanged)
        Q_PROPERTY(int itemsCount READ getItemsCount NOTIFY itemsCountChanged)
        Q_OBJECT
    public:
        ArtworkUploader(Connectivity::IFtpCoordinator *ftpCoordinator, QObject *parent=0);
        virtual ~ArtworkUploader();

    public:
        // used to test UI of artwork upload
        // virtual bool getInProgress() const { return true; }
        AutoComplete::StringsAutoCompleteModel *getStocksCompletionSource() { return &m_StocksCompletionSource; }
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;

    signals:
        void inProgressChanged();
        void hasErrorsChanged();
        void percentChanged();
        void itemsCountChanged();
        void startedProcessing();
        void finishedProcessing();
        void requestCloseWindow();
        void credentialsChecked(bool result, const QString &url);

    public:
        int getUIPercent() const { return ((PERCENT_EPSILON < m_Percent) && (m_Percent < 1.0)) ? 1 : (int)m_Percent; }
        bool getInProgress() const { return m_IsInProgress; }
        bool getHasErrors() const { return m_HasErrors; }
        int getItemsCount() const { return (int)m_ArtworksSnapshot.size(); }

    public:
        void setPercent(double value);
        void setInProgress(bool value);
        void setHasErrors(bool value);

    public slots:
        void onUploadStarted();
        void allFinished(bool anyError);
        void credentialsTestingFinished();

    private slots:
        void uploaderPercentChanged(double percent);
        void stocksListUpdated();
        void updateStocksList();

    public:
        Q_INVOKABLE void uploadArtworks();
        Q_INVOKABLE void checkCredentials(const QString &host, const QString &username,
                                          const QString &password, bool disablePassiveMode, bool disableEPSV);
        Q_INVOKABLE bool needCreateArchives() const;

        Q_INVOKABLE QString getFtpAddress(const QString &stockName) const { return m_StocksFtpList.getFtpAddress(stockName); }
        Q_INVOKABLE QString getFtpName(const QString &stockAddress) const;

        Q_INVOKABLE QObject *getUploadWatcher();

        Q_INVOKABLE void resetModel();
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void resetProgress();
        Q_INVOKABLE void cancelOperation();

        void initializeStocksList(Helpers::AsyncCoordinator *initCoordinator);

    public:
        void setArtworks(MetadataIO::ArtworksSnapshot &snapshot);
        void resetArtworks();

#ifdef CORE_TESTS
    public:
#else
    protected:
#endif
        const MetadataIO::ArtworksSnapshot &getArtworksSnapshot() const { return m_ArtworksSnapshot; }

#ifdef INTEGRATION_TESTS
    public:
        Connectivity::UploadWatcher *accessWatcher() { return &m_UploadWatcher; }
#endif

    private:
        void doUploadArtworks(const MetadataIO::ArtworksSnapshot &snapshot);

    protected:
        virtual bool removeUnavailableItems() override;

    private:
        MetadataIO::ArtworksSnapshot m_ArtworksSnapshot;
        Connectivity::UploadWatcher m_UploadWatcher;
        Connectivity::IFtpCoordinator *m_FtpCoordinator;
        AutoComplete::StringsAutoCompleteModel m_StocksCompletionSource;
        AutoComplete::StocksFtpListModel m_StocksFtpList;
        QFutureWatcher<Connectivity::ContextValidationResult> *m_TestingCredentialWatcher;
        double m_Percent;
        volatile bool m_IsInProgress;
        volatile bool m_HasErrors;
    };
}

#endif // ARTWORKUPLOADER_H
