/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef STRINGSAUTOCOMPLETEMODEL_H
#define STRINGSAUTOCOMPLETEMODEL_H

#include <QSortFilterProxyModel>
#include <QStringListModel>
#include "autocompletemodel.h"

namespace AutoComplete {
    // designed to use only with QStringListModel
    class StringFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        StringFilterProxyModel();

    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        bool setSearchTerm(const QString &value);

    public:
        void setStrings(const QStringList &list);
        QString getString(int index);
        int acceptCompletion(int index);

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
    };

    class StringsAutoCompleteModel: public AutoCompleteModel
    {
        Q_OBJECT
        Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    public:
        StringsAutoCompleteModel();

    public:
        const QString &getSearchTerm() { return m_StringsModel.getSearchTerm(); }
        void setSearchTerm(const QString &value) {
            if (m_StringsModel.setSearchTerm(value)) {
                emit searchTermChanged(value);
            }
        }

    public:
        void setStrings(const QStringList &list) { m_StringsModel.setStrings(list); }

    signals:
        void searchTermChanged(const QString &value);

        // AutoCompleteModel interface
    protected:
        virtual int getCompletionsCount() override;
        virtual QString doGetCompletion(int completionID) override;
        virtual QAbstractItemModel *doGetCompletionsModel() override;
        virtual void doInitializeCompletions() override;
        virtual void clearCompletions() override;
        virtual int doAcceptCompletion(int index, bool withMetaAction) override;

    private:
        StringFilterProxyModel m_StringsModel;
    };
}

#endif // STRINGSAUTOCOMPLETEMODEL_H
