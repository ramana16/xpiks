/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "stringsautocompletemodel.h"
#include "../Helpers/stringhelper.h"
#include "../Common/defines.h"

namespace AutoComplete {
    StringFilterProxyModel::StringFilterProxyModel():
        QSortFilterProxyModel(),
        m_Threshold(80)
    {
        setSourceModel(&m_StringsModel);
    }

    bool StringFilterProxyModel::setSearchTerm(const QString &value) {
        bool anyChange = false;

        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            anyChange = true;
        }

        invalidateFilter();

        return anyChange;
    }

    void StringFilterProxyModel::setStrings(const QStringList &list) {
        LOG_INFO << "Adding" << list.length() << "values";
        m_StringsList = list;
        m_StringsModel.setStringList(m_StringsList);
    }

    QString StringFilterProxyModel::getString(int index) {
        QString result;

        if ((0 <= index) && (index < m_StringsList.size())) {
            result = m_StringsList.at(index);
        }

        return result;
    }

    int StringFilterProxyModel::acceptCompletion(int index) {
        QModelIndex proxyIndex = this->index(index, 0);
        QModelIndex originalIndex = this->mapToSource(proxyIndex);
        const int realIndex = originalIndex.row();

        LOG_DEBUG << "Real index:" << realIndex;
        return realIndex;
    }

    bool StringFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);
        if (sourceRow < 0 || sourceRow >= m_StringsList.length()) { return false; }
        if (m_SearchTerm.trimmed().isEmpty()) { return true; }

        QString item = m_StringsList.at(sourceRow).toLower();
        if (item.contains(m_SearchTerm, Qt::CaseInsensitive)) { return true; }

        int percentsSimilar = Helpers::levensteinPercentage(item, m_SearchTerm);
        int percentageThreshold = m_Threshold;

        if (m_SearchTerm.length() < item.length()) {
            percentageThreshold = (m_Threshold * m_SearchTerm.length()) / item.length();
        }

        return (percentsSimilar >= percentageThreshold);
    }

    QHash<int, QByteArray> StringFilterProxyModel::roleNames() const {
        auto names = QSortFilterProxyModel::roleNames();
        names[Qt::UserRole + 1] = "ispreset";
        return names;
    }

    QVariant StringFilterProxyModel::data(const QModelIndex &index, int role) const {
        // FIXME:
        if (role == (Qt::UserRole + 1)) {
            // yes, this is ugly hack with presets
            return false;
        }

        return QSortFilterProxyModel::data(index, role);
    }

    StringsAutoCompleteModel::StringsAutoCompleteModel()
    {
    }

    int StringsAutoCompleteModel::getCompletionsCount() {
        const int count = m_StringsModel.rowCount();
        return count;
    }

    QString StringsAutoCompleteModel::doGetCompletion(int completionID) {
        return m_StringsModel.getString(completionID);
    }

    QAbstractItemModel *StringsAutoCompleteModel::doGetCompletionsModel() {
        return &m_StringsModel;
    }

    void StringsAutoCompleteModel::doInitializeCompletions() { /*bump*/ }

    void StringsAutoCompleteModel::clearCompletions() { /*bump*/ }

    int StringsAutoCompleteModel::doAcceptCompletion(int index, bool withMetaAction) {
        Q_UNUSED(withMetaAction);
        return m_StringsModel.acceptCompletion(index);
    }
}
