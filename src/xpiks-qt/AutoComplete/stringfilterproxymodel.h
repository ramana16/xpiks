/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef STRINGFILTERPROXYMODEL_H
#define STRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QString>

namespace AutoComplete {
    // designed to use only with QStringListModel
    class StringFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
        Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
        Q_PROPERTY(int selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
        Q_PROPERTY(bool isActive READ getIsActive WRITE setIsActive NOTIFY isActiveChanged)
    public:
        StringFilterProxyModel();

    public:
        int getSelectedIndex() const { return m_SelectedIndex; }
        bool getIsActive() const { return m_IsActive; }
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);
        void setSelectedIndex(int value) {
            if (value != m_SelectedIndex) {
                m_SelectedIndex = value;
                emit selectedIndexChanged(value);
            }
        }

        void setIsActive(bool value) {
            if (value != m_IsActive) {
                m_IsActive = value;
                emit isActiveChanged(value);
            }
        }

    public:
        void setStrings(const QStringList &list);
        Q_INVOKABLE bool moveSelectionUp();
        Q_INVOKABLE bool moveSelectionDown();
        Q_INVOKABLE void cancelCompletion() { setIsActive(false); emit dismissPopupRequested(); }
        Q_INVOKABLE void acceptSelected();
        Q_INVOKABLE bool hasSelectedCompletion() { return (0 <= m_SelectedIndex) && (m_SelectedIndex < rowCount()); }

    signals:
        void searchTermChanged(const QString &value);
        void selectedIndexChanged(int index);
        void isActiveChanged(bool value);
        void dismissPopupRequested();
        void completionAccepted(const QString &completion, bool expandPreset);

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
        // for hack with presets
        virtual QHash<int,QByteArray> roleNames() const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        // ignore default regexp from proxymodel
        QString m_SearchTerm;
        QStringListModel m_StringsModel;
        QStringList m_StringsList;
        int m_Threshold;
        int m_SelectedIndex;
        bool m_IsActive;
    };
}

#endif // STRINGFILTERPROXYMODEL_H
