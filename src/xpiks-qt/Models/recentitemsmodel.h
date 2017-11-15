/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef RECENTITEMSMODEL_H
#define RECENTITEMSMODEL_H

#include <QAbstractListModel>
#include <QQueue>
#include <QString>
#include <QSet>
#include <QHash>
#include <QUrl>

namespace Models {
    class RecentItemsModel: public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(int count READ getRecentItemsCount NOTIFY recentItemsCountChanged)
    public:
        RecentItemsModel(int items);

    public:
        int getMaxRecentItems() const { return m_MaxRecentItems; }
        int getRecentItemsCount() const { return m_RecentItems.size(); }

    public:
        Q_INVOKABLE QString serializeForSettings();
        Q_INVOKABLE QUrl getLatestItem() const;

    public:
        void deserializeFromSettings(const QString &serialized);
        void pushItem(const QString &directoryPath);
        QStringList getAllRecentFiles();

#ifdef CORE_TESTS
    public:
        QString getLatestUsedItem() const { return m_LatestUsedItem; }
#endif

    private:
        bool doPushItem(const QString &itemPath);

    public:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override { Q_UNUSED(parent); return m_RecentItems.length(); }
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override { return QAbstractListModel::roleNames(); }

    public slots:
        void onRecentItemsUpdated(const QString &serialized);

    signals:
        void recentItemsCountChanged();

    private:
        QSet<QString> m_ItemsSet;
        QQueue<QString> m_RecentItems;
        int m_MaxRecentItems;
        QString m_LatestUsedItem;
    };
}

#endif // RECENTITEMSMODEL_H
