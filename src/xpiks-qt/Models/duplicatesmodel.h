/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DUPLICATESMODEL_H
#define DUPLICATESMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QQmlEngine>
#include <QReadWriteLock>
#include "../Common/baseentity.h"
#include "../Common/imetadataoperator.h"
#include "../Common/hold.h"

namespace Models {
    class DuplicatesModel;

    class SelectableKeywordsModel:
        public QAbstractListModel
    {
    Q_OBJECT

    public:
        SelectableKeywordsModel(const QString &stem, const QVector<int> &keywordsIndexes, DuplicatesModel *duplicatesModel);

// QAbstractListModel

    protected:
        enum Roles {
            KeywordRole = Qt::UserRole + 1,
            IsSelectedRole
        };

    public:
        const QString& getStem() const { return m_Stem; }
        void notifySelectedChanged(int index);

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    private:
        QString m_Stem;
        QVector<int> m_KeywordsIndexes;
        DuplicatesModel *m_DuplicatesModel;
    };

    class DuplicatesModel:
        public QAbstractListModel
    {
    Q_OBJECT

    public:
        DuplicatesModel(QObject *parent=0);
        void setupModel(Common::IMetadataOperator *model);

    public:
        Q_INVOKABLE QObject *getSelectableKeywordsModel(int index);
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void saveChanges();

// QAbstractListModel
    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    public :
        bool setSelected(int index, bool value);
        bool getSelected(int index);
        QString getKeyword(int index);

    private:
        void notifySelectedChanged(int index);

    private:
        QVector<std::pair<QString, bool>> m_KeywordsData;
        Common::IMetadataOperator *m_ExternalModel;
        std::vector<std::shared_ptr<SelectableKeywordsModel> > m_KeywordsPerStem;
    };
}

Q_DECLARE_METATYPE(Models::SelectableKeywordsModel *)

#endif // DUPLICATESMODEL_H
