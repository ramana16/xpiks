/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef STOCKSFTPLISTMODEL_H
#define STOCKSFTPLISTMODEL_H

#include <QHash>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include "../Models/abstractconfigupdatermodel.h"

namespace AutoComplete {
    class StocksFtpListModel: public Models::AbstractConfigUpdaterModel
    {
        Q_OBJECT
    public:
        StocksFtpListModel();

    public:
        void initializeConfigs();
        QString getFtpAddress(const QString &stockName) const { return m_StocksHash.value(stockName, ""); }
        const QStringList &getStockNamesList() const { return m_StockNames; }

        // AbstractConfigUpdaterModel interface
    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) override;
        virtual bool processLocalConfig(const QJsonDocument &document) override;
        virtual void processMergedConfig(const QJsonDocument &document) override;

    private:
        void parseConfig(const QJsonDocument &document);
        void parseFtpArray(const QJsonArray &array);

    signals:
        void stocksListUpdated();

        // CompareValuesJson interface
    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2) override;

    private:
        QHash<QString, QString> m_StocksHash;
        QStringList m_StockNames;
    };
}

#endif // STOCKSFTPLISTMODEL_H
