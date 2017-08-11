/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "stringfilterproxymodel.h"
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QString>
#include "../Helpers/stringhelper.h"
#include "../Common/defines.h"

namespace AutoComplete {
    StringFilterProxyModel::StringFilterProxyModel():
        m_Threshold(2),
        m_SelectedIndex(-1),
        m_IsActive(false)
    {
        setSourceModel(&m_StringsModel);
    }

    void StringFilterProxyModel::setSearchTerm(const QString &value) {
        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
    }

    void StringFilterProxyModel::setStrings(const QStringList &list) {
        LOG_INFO << "Adding" << list.length() << "values";
        m_StringsList = list;
        m_StringsModel.setStringList(m_StringsList);
    }

    bool StringFilterProxyModel::moveSelectionUp() {
        bool canMove = m_SelectedIndex > 0;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex - 1);
        }
        return canMove;
    }

    bool StringFilterProxyModel::moveSelectionDown() {
        bool canMove = m_SelectedIndex < rowCount() - 1;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex + 1);
        }
        return canMove;
    }

    void StringFilterProxyModel::acceptSelected() {
        LOG_DEBUG << "Selected index:" << m_SelectedIndex;

        if (0 <= m_SelectedIndex && m_SelectedIndex < rowCount()) {
            QModelIndex proxyIndex = this->index(m_SelectedIndex, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);
            int index = originalIndex.row();

            LOG_DEBUG << "Real index:" << index;

            emit completionAccepted(m_StringsList.at(index), false);
        }

        emit dismissPopupRequested();

        setIsActive(false);
    }

    bool StringFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);
        if (sourceRow < 0 || sourceRow >= m_StringsList.length()) { return false; }
        if (m_SearchTerm.trimmed().isEmpty()) { return true; }

        QString item = m_StringsList.at(sourceRow).toLower();
        if (item.contains(m_SearchTerm, Qt::CaseInsensitive)) { return true; }

        int distance = Helpers::levensteinDistance(item.left(m_SearchTerm.length() + m_Threshold - 1), m_SearchTerm);
        return distance <= m_Threshold;
    }

    QHash<int, QByteArray> StringFilterProxyModel::roleNames() const {
        auto names = QSortFilterProxyModel::roleNames();
        names[Qt::UserRole + 1] = "ispreset";
        return names;
    }

    QVariant StringFilterProxyModel::data(const QModelIndex &index, int role) const {
        if (role == (Qt::UserRole + 1)) {
            return false;
        }

        return QSortFilterProxyModel::data(index, role);
    }
}
