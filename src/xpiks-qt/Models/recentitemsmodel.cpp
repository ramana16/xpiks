/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "recentitemsmodel.h"
#include <QDataStream>
#include <QUrl>
#include "../Common/defines.h"

namespace Models {
    RecentItemsModel::RecentItemsModel():
        QAbstractListModel()
    {
    }

    QString RecentItemsModel::serializeForSettings() {
        QByteArray raw;
        QDataStream ds(&raw, QIODevice::WriteOnly);
        ds << m_RecentItems;
        return QString::fromLatin1(raw.toBase64());
    }

    QUrl RecentItemsModel::getLatestItem() const {
        return QUrl::fromLocalFile(m_LatestUsedItem);
    }

    void RecentItemsModel::deserializeFromSettings(const QString &serialized) {
        LOG_DEBUG << "#";

        QByteArray originalData;
        originalData.append(serialized.toLatin1());
        QByteArray serializedBA = QByteArray::fromBase64(originalData);

        QQueue<QString> items;
        QDataStream ds(&serializedBA, QIODevice::ReadOnly);
        ds >> items;

        QQueue<QString> deserialized;
        QSet<QString> toBeAdded;

        foreach (const QString &item, items) {
            if (!toBeAdded.contains(item)) {
                toBeAdded.insert(item);
                deserialized.push_back(item);
            }
        }

        m_ItemsSet = toBeAdded;
        m_RecentItems = deserialized;
    }

    void RecentItemsModel::pushItem(const QString &itemPath) {
        if (doPushItem(itemPath)) {
            LOG_DEBUG << "Added new recent item";
        }

        m_LatestUsedItem = itemPath;

        emit recentItemsCountChanged();
    }

    QStringList RecentItemsModel::getAllRecentFiles() {
        QStringList items;
        items.reserve(m_RecentItems.size());

        for (auto &item: m_RecentItems) {
            items.push_back(item);
        }

        return items;
    }

    bool RecentItemsModel::doPushItem(const QString &itemPath) {
        bool added = false;

        if (!m_ItemsSet.contains(itemPath)) {
            m_ItemsSet.insert(itemPath);

            int length = m_RecentItems.length();
            beginInsertRows(QModelIndex(), length, length);
            m_RecentItems.push_back(itemPath);
            endInsertRows();

            if (m_RecentItems.length() > m_MaxRecentItems) {
                QString itemToRemove = m_RecentItems.first();
                beginRemoveRows(QModelIndex(), 0, 0);
                m_RecentItems.pop_front();
                endRemoveRows();
                m_ItemsSet.remove(itemToRemove);
            }

            added = true;
        }

        return added;
    }

    QVariant RecentItemsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_RecentItems.length()) return QVariant();
        if (role == Qt::DisplayRole) { return m_RecentItems.at(index.row()); }
        return QVariant();
    }

    void RecentItemsModel::onRecentItemsUpdated(const QString &serialized) {
        LOG_DEBUG << "#";
        beginResetModel();
        {
            deserializeFromSettings(serialized);
        }
        endResetModel();

        emit recentItemsCountChanged();
    }
}
