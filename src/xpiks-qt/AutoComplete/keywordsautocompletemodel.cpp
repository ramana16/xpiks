/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "keywordsautocompletemodel.h"
#include "../Common/defines.h"

namespace AutoComplete {
    KeywordsCompletionsModel::KeywordsCompletionsModel() {
        QObject::connect(this, &KeywordsCompletionsModel::updateRequired,
                         this, &KeywordsCompletionsModel::onUpdateRequired);
    }

    void KeywordsCompletionsModel::setKeywordCompletions(const std::vector<CompletionResult> &completions) {
        Q_ASSERT(!completions.empty());
        LOG_INFO << completions.size() << "completions";

        QMutexLocker locker(&m_CompletionsLock);
        Q_UNUSED(locker);

        m_LastGeneratedCompletions.clear();
        m_LastGeneratedCompletions.reserve(completions.size());

        for (auto &result: completions) {
            const int id = m_LastCompletionID.fetchAndAddOrdered(1);
            m_LastGeneratedCompletions.emplace_back(new CompletionItem(result.m_Completion, id));
            m_LastGeneratedCompletions.back()->setIsKeyword();
        }
    }

    void KeywordsCompletionsModel::setPresetCompletions(const std::vector<CompletionResult> &completions) {
        Q_ASSERT(!completions.empty());
        LOG_INFO << completions.size() << "completions";

        QMutexLocker locker(&m_CompletionsLock);
        Q_UNUSED(locker);

        m_LastGeneratedCompletions.clear();
        m_LastGeneratedCompletions.reserve(completions.size());

        for (auto &result: completions) {
            const int id = m_LastCompletionID.fetchAndAddOrdered(1);
            m_LastGeneratedCompletions.emplace_back(new CompletionItem(result.m_Completion, id));
            m_LastGeneratedCompletions.back()->setIsPreset();
            m_LastGeneratedCompletions.back()->setPresetID(result.m_PresetID);
        }
    }

    void KeywordsCompletionsModel::setPresetsMembership(const std::vector<CompletionResult> &completions) {
        LOG_DEBUG << "#";
        bool anyChanged = false;
        {
            QMutexLocker locker(&m_CompletionsLock);
            Q_UNUSED(locker);

            if (completions.size() != m_LastGeneratedCompletions.size()) {
                LOG_WARNING << "Completions have changed";
                return;
            }

            const size_t size = m_LastGeneratedCompletions.size();
            for (size_t i = 0; i < size; i++) {
                auto &lastCompletion = m_LastGeneratedCompletions[i];
                auto &updatedCompletion = completions[i];

                if (lastCompletion->getCompletion() == updatedCompletion.m_Completion) {
                    if (updatedCompletion.m_PresetID != -1) {
                        LOG_INTEGR_TESTS_OR_DEBUG << "Completion" << i << "is preset with ID" << updatedCompletion.m_PresetID;
                        lastCompletion->setPresetID(updatedCompletion.m_PresetID);
                        lastCompletion->setCanBePreset();
                        anyChanged = true;
                    }
                }
            }
        }

        if (anyChanged) {
            emit updateRequired();
        }
    }

    std::shared_ptr<CompletionItem> KeywordsCompletionsModel::getAcceptedCompletion(int completionID) {
        auto result = m_AcceptedCompletions.value(completionID, std::shared_ptr<CompletionItem>());
        return result;
    }

    QString KeywordsCompletionsModel::getCompletion(int completionID) {
        QString result;

        auto completionItem = m_AcceptedCompletions.value(completionID, std::shared_ptr<CompletionItem>());
        if (completionItem) {
            result = completionItem->getCompletion();
        }

        return result;
    }

    void KeywordsCompletionsModel::sync() {
        LOG_DEBUG << "#";

        QMutexLocker locker(&m_CompletionsLock);
        Q_UNUSED(locker);

        LOG_DEBUG << "Setting current completions to" << m_LastGeneratedCompletions.size() << "item(s)";

        beginResetModel();
        {
            m_CompletionList = m_LastGeneratedCompletions;
        }
        endResetModel();
    }

    void KeywordsCompletionsModel::clear() {
        QMutexLocker locker(&m_CompletionsLock);
        Q_UNUSED(locker);

        beginResetModel();
        {
            m_CompletionList.clear();
            m_LastGeneratedCompletions.clear();
            m_AcceptedCompletions.clear();
        }
        endResetModel();
    }

    int KeywordsCompletionsModel::acceptCompletion(int index, bool withMetaAction) {
        auto &item = m_CompletionList.at(index);
        if (withMetaAction) {
            item->setShouldExpandPreset();
        }

        int id = item->getID();
        m_AcceptedCompletions.insert(id, item);
        return id;
    }

    QVariant KeywordsCompletionsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_CompletionList.size()) return QVariant();

        auto &item = m_CompletionList[row];
        if (role == Qt::DisplayRole) { return item->getCompletion(); }
        else if (role == IsPresetRole) { return item->canBePreset(); }
        return QVariant();
    }

    QHash<int, QByteArray> KeywordsCompletionsModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[IsPresetRole] = "ispreset";
        return roles;
    }

    void KeywordsCompletionsModel::onUpdateRequired() {
        LOG_DEBUG << "Updating completions...";

        QModelIndex first = this->index(0);
        QModelIndex last = this->index(rowCount() - 1);

        emit dataChanged(first, last, QVector<int>() << IsPresetRole);
    }

#ifdef INTEGRATION_TESTS
    bool KeywordsCompletionsModel::containsWord(const QString &word) const {
        bool contains = false;
        for (auto &item: m_CompletionList) {
            if (item->getCompletion() == word) {
                contains = true;
                break;
            }
        }

        return contains;
    }

    QStringList KeywordsCompletionsModel::getLastGeneratedCompletions() {
        QStringList completions;
        for (auto &item: m_CompletionList) {
            completions.append(item->getCompletion());
        }
        return completions;
    }
#endif

    KeywordsAutoCompleteModel::KeywordsAutoCompleteModel()
    {
    }

    int KeywordsAutoCompleteModel::getCompletionsCount() {
        return m_CompletionsModel.rowCount();
    }

    QString KeywordsAutoCompleteModel::doGetCompletion(int completionID) {
        return m_CompletionsModel.getCompletion(completionID);
    }

    QAbstractItemModel *KeywordsAutoCompleteModel::doGetCompletionsModel() {
        return &m_CompletionsModel;
    }

    void KeywordsAutoCompleteModel::doInitializeCompletions() {
        m_CompletionsModel.sync();
    }

    void KeywordsAutoCompleteModel::clearCompletions() {
        m_CompletionsModel.clear();
    }

    int KeywordsAutoCompleteModel::doAcceptCompletion(int index, bool withMetaAction) {
        return m_CompletionsModel.acceptCompletion(index, withMetaAction);
    }
}
