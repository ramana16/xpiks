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
#include "artworksprocessor.h"
#include "../Connectivity/testconnection.h"
#include "../AutoComplete/stringfilterproxymodel.h"
#include "../AutoComplete/stocksftplistmodel.h"
#include "../Connectivity/uploadwatcher.h"

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
    class ArtworkMetadata;

    class ArtworkUploader: public ArtworksProcessor
    {
        Q_OBJECT
    public:
        ArtworkUploader(Connectivity::IFtpCoordinator *ftpCoordinator, QObject *parent=0);
        virtual ~ArtworkUploader();

    public:
        // used to test UI of artwork upload
        // virtual bool getInProgress() const { return true; }
        AutoComplete::StringFilterProxyModel *getStocksCompletionSource() { return &m_StocksCompletionSource; }
        virtual void setCommandManager(Commands::CommandManager *commandManager) override;

    signals:
        void percentChanged();
        void credentialsChecked(bool result, const QString &url);

    public:
        virtual int getPercent() const override { return m_Percent; }

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
                                          const QString &password, bool disablePassiveMode, bool disableEPSV) const;
        Q_INVOKABLE bool needCreateArchives() const;

        Q_INVOKABLE QString getFtpAddress(const QString &stockName) const { return m_StocksFtpList.getFtpAddress(stockName); }
        Q_INVOKABLE QString getFtpName(const QString &stockAddress) const;

        Q_INVOKABLE QObject *getUploadWatcher() {
            auto *model = &m_UploadWatcher;
            QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);

            return model;
        }

        void initializeStocksList(Helpers::AsyncCoordinator *initCoordinator);

    private:
        void doUploadArtworks(const MetadataIO::ArtworksSnapshot &snapshot);

    protected:
        virtual void cancelProcessing() override;

        virtual void innerResetModel() override { m_Percent = 0; m_UploadWatcher.resetModel(); }

    private:
        Connectivity::UploadWatcher m_UploadWatcher;
        Connectivity::IFtpCoordinator *m_FtpCoordinator;
        AutoComplete::StringFilterProxyModel m_StocksCompletionSource;
        AutoComplete::StocksFtpListModel m_StocksFtpList;
        QFutureWatcher<Connectivity::ContextValidationResult> *m_TestingCredentialWatcher;
        int m_Percent;
    };
}

#endif // ARTWORKUPLOADER_H
