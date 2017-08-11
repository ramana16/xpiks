/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WARNINGSMODEL_H
#define WARNINGSMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Models/artitemsmodel.h"

namespace Warnings {
    class WarningsSettingsModel;

    class WarningsModel: public QSortFilterProxyModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int warningsCount READ getWarningsCount NOTIFY warningsCountChanged)
    public:
        WarningsModel(QObject *parent=0);

    public:
        void setWarningsSettingsModel(const WarningsSettingsModel *warningsSettingsModel) {
            m_WarningsSettingsModel = warningsSettingsModel;
            Q_ASSERT(warningsSettingsModel != nullptr);
        }

    public:
        int getWarningsCount() const { return rowCount(); }

    public:
        Q_INVOKABLE void setShowSelected() { m_ShowOnlySelected = true; update(); }
        Q_INVOKABLE void resetShowSelected() { m_ShowOnlySelected = false; update(); }
        Q_INVOKABLE QStringList describeWarnings(int index) const;
        Q_INVOKABLE void update();
        Q_INVOKABLE int getOriginalIndex(int index) const;
        Q_INVOKABLE void processPendingUpdates();

    signals:
        void warningsCountChanged();

    public slots:
        void onWarningsCouldHaveChanged(int originalIndex);
        void onWarningsUpdateRequired();

    private slots:
        void sourceRowsRemoved(QModelIndex,int,int);
        void sourceRowsInserted(QModelIndex,int,int);
        void sourceModelReset();

    private:
        enum WarningsModel_Roles {
            WarningsRole = Models::ArtItemsModel::RolesNumber + 1
        };

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    public:
        virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        const WarningsSettingsModel *m_WarningsSettingsModel;
        QVector<int> m_PendingUpdates;
        bool m_ShowOnlySelected;
    };
}

#endif // WARNINGSMODEL_H
