/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "duplicatesmodel.h"
#include "../Commands/commandmanager.h"
#include <algorithm>
#include <QQmlEngine>

namespace Models {
    SelectableKeywordsModel::SelectableKeywordsModel(const QString &stem, const QVector<int> &keywordsIndexes, DuplicatesModel *duplicatesModel):
        m_Stem(stem),
        m_KeywordsIndexes(keywordsIndexes),
        m_DuplicatesModel(duplicatesModel)
    {
        Q_ASSERT(duplicatesModel != nullptr);
    }

    void SelectableKeywordsModel::notifySelectedChanged(int index) {
        int i = m_KeywordsIndexes.indexOf(index);

        if (i >= 0) {
            QModelIndex modelIndex = this->index(i);
            emit dataChanged(modelIndex, modelIndex, QVector<int>() << IsSelectedRole);
        }
    }

    QHash<int, QByteArray> SelectableKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    int SelectableKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsIndexes.size();
    }

    QVariant SelectableKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= (int)m_KeywordsIndexes.size()) {
            return QVariant();
        }

        int keywordIndex = m_KeywordsIndexes.at(row);

        switch (role) {
            case KeywordRole:
                return m_DuplicatesModel->getKeyword(keywordIndex);
            case IsSelectedRole:
                return m_DuplicatesModel->getSelected(keywordIndex);
            default:
                return QVariant();
        }
    }

    bool SelectableKeywordsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        LOG_DEBUG << '#';
        int row = index.row();

        if (row < 0 || row >= (int)m_KeywordsIndexes.size()) {
            return false;
        }

        int keywordIndex = m_KeywordsIndexes[row];
        bool result = false;

        switch (role) {
            case IsSelectedRole:
                if (m_DuplicatesModel->setSelected(keywordIndex, value.toBool())) {
                    emit dataChanged(index, index, QVector<int>() << IsSelectedRole);
                }

                result = true;
                break;
            default:
                break;
        }

        return result;
    }

    Qt::ItemFlags SelectableKeywordsModel::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || row >= m_KeywordsIndexes.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    DuplicatesModel::DuplicatesModel(QObject *parent):
        QAbstractListModel(parent)
        {}

    void DuplicatesModel::setupModel(Common::IMetadataOperator *model) {
        beginResetModel();
        LOG_DEBUG << '#';
        m_KeywordsData.clear();
        m_ExternalModel = model;
        m_KeywordsPerStem.clear();
        const auto &stemsWithDuplicates = model->getDuplicatesModel();
        QStringList duplicatesList;
        for (const auto &duplicates : stemsWithDuplicates) {
            duplicatesList.append(duplicates);
        }

        duplicatesList.removeDuplicates();
        m_KeywordsData.reserve(duplicatesList.size());
        for (const auto &duplicate : duplicatesList) {
            m_KeywordsData.append({duplicate, false});
        }

        const auto begin = stemsWithDuplicates.begin();
        const auto end = stemsWithDuplicates.end();
        for (auto it = begin; it != end; it++) {
            const auto &stem = it.key();
            const auto &duplicates = it.value();
            QVector<int> indexes;
            indexes.reserve(duplicates.size());
            for (const auto &duplicate : duplicates) {
                int index = duplicatesList.indexOf(duplicate);
                if (index >= 0) {
                    indexes.push_back(index);
                }
            }

            m_KeywordsPerStem.emplace_back(new SelectableKeywordsModel(stem, indexes, this));
        }

        endResetModel();
    }

    QObject *DuplicatesModel::getSelectableKeywordsModel(int index) {
        QObject *result = nullptr;
        size_t size = m_KeywordsPerStem.size();

        if (0 <= index && index < size) {
            QObject *item = m_KeywordsPerStem[index].get();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            result = item;
        }

        return result;
    }

    void DuplicatesModel::clearModel() {
        m_ExternalModel = nullptr;
        m_KeywordsPerStem.clear();
    }

    int DuplicatesModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return int(m_KeywordsPerStem.size());
    }

    QVariant DuplicatesModel::data(const QModelIndex &index, int role) const {
        Q_UNUSED(index);
        Q_UNUSED(role);
        return QVariant();
    }

    Qt::ItemFlags DuplicatesModel::flags(const QModelIndex &index) const
    {
        int row = index.row();

        if (row < 0 || row >= m_KeywordsData.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool DuplicatesModel::setSelected(int index, bool value) {
        int size = m_KeywordsData.size();

        if (0 <= index && index < size) {
            {
                m_KeywordsData[index].second = value;
            }
            notifySelectedChanged(index);
        }

        return false;
    }

    bool DuplicatesModel::getSelected(int index) {
        int size = m_KeywordsData.size();

        if (0 <= index && index < size) {
            return m_KeywordsData[index].second;
        }

        return false;
    }

    QString DuplicatesModel::getKeyword(int index) {
        int size = m_KeywordsData.size();

        if (0 <= index && index < size) {
            return m_KeywordsData[index].first;
        }

        return QString();
    }

    void DuplicatesModel::notifySelectedChanged(int index) {
        for (const auto &model : m_KeywordsPerStem) {
            if (model) {
                model->notifySelectedChanged(index);
            }
        }
    }

    void DuplicatesModel::saveChanges() {
        QStringList keywordsToRemove;

        for (const auto &keyword : m_KeywordsData) {
            bool selected = keyword.second;
            if (selected) {
                keywordsToRemove.append(keyword.first);
            }
        }

        if (m_ExternalModel != nullptr) {
            const auto wordsSet = QSet<QString>::fromList(keywordsToRemove);
            m_ExternalModel->removeKeywords(wordsSet, true);
        }
    }
}
