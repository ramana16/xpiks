/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "presetkeywordsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Helpers/stringhelper.h"
#include "presetmodel.h"

#define MAX_SAVE_PAUSE_RESTARTS 5
#define PRESET_SAVE_TIMEOUT 3000

namespace KeywordsPresets {
    PresetKeywordsModel::PresetKeywordsModel(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        Common::DelayedActionEntity(PRESET_SAVE_TIMEOUT, MAX_SAVE_PAUSE_RESTARTS),
        m_GroupsModel(this),
        m_LastUsedID(0)
    {
        m_SavingTimer.setSingleShot(true);
        QObject::connect(&m_SavingTimer, &QTimer::timeout, this, &PresetKeywordsModel::onSavingTimerTriggered);

        QObject::connect(this, &PresetKeywordsModel::backupRequest, this, &PresetKeywordsModel::onBackupRequested);
        QObject::connect(&m_GroupsModel, &PresetGroupsModel::backupRequested, this, &PresetKeywordsModel::backupRequest);
    }

    PresetKeywordsModel::~PresetKeywordsModel() {
        removeAllPresets();

        for (auto *x: m_Finalizers) {
            delete x;
        }
    }

    void PresetKeywordsModel::initializePresets() {
        LOG_DEBUG << "#";
        m_PresetsConfig.initializeConfigs();
        loadModelFromConfig();
        requestBackup();
    }

    bool PresetKeywordsModel::tryGetNameFromIndex(int index, QString &name) {
        if (index < 0 || index >= getPresetsCount()) {
            return false;
        }

        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);
        bool result = tryGetNameFromIndexUnsafe(index, name);
        return result;
    }

    bool PresetKeywordsModel::tryGetGroupFromIndex(int index, int &groupID) {
        if (index < 0 || index >= getPresetsCount()) {
            return false;
        }

        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);
        bool result = tryGetGroupFromIndexUnsafe(index, groupID);
        return result;
    }

#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
    void PresetKeywordsModel::setName(int presetIndex, const QString &name) {
        if (presetIndex < 0 || presetIndex >= getPresetsCount()) {
            return;
        }

        m_PresetsList[presetIndex]->m_PresetName = name;
    }
    ID_t PresetKeywordsModel::addItem(const QString &presetName, const QStringList &keywords) {
        LOG_DEBUG << "#";
        int lastIndex = getPresetsCount();
        int nextID = generateNextID();

        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        {
            m_PresetsList.push_back(new PresetModel(nextID, presetName, keywords, DEFAULT_GROUP_ID));
        }
        endInsertRows();

        return m_PresetsList.back()->m_ID;
    }

    void PresetKeywordsModel::cleanup() {
         beginResetModel();
         {
             removeAllPresets();
         }
         endResetModel();
    }

    bool PresetKeywordsModel::removePresetByID(ID_t id) {
        LOG_INFO << id;
        bool result = false;

        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        size_t index = 0;
        if (tryGetPresetIndexByIDUnsafe(id, index)) {
            result = removeItemUnsafe(index);
        }

        return result;
    }
#endif

    bool PresetKeywordsModel::tryGetPreset(ID_t id, QStringList &keywords) {
        bool result = false;

        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        size_t index = 0;

        if (tryGetPresetIndexByIDUnsafe(id, index)) {
            result = tryGetPresetUnsafe(index, keywords);
        }

        return result;
    }

    bool PresetKeywordsModel::tryFindSinglePresetByName(const QString &name, bool strictMatch, ID_t &id) {
        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);
        size_t index = 0;
        bool result = false;

        if (tryFindSinglePresetByNameUnsafe(name, strictMatch, index)) {
            id = m_PresetsList[index]->m_ID;
            result = true;
        }

        return result;
    }

    void PresetKeywordsModel::findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) {
        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);
        findPresetsByNameUnsafe(name, results);
    }

    void PresetKeywordsModel::findOrRegisterPreset(const QString &name, const QStringList &keywords, ID_t &id) {
        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        size_t index = 0;
        findOrRegisterPresetUnsafe(name, keywords, index);
        id = m_PresetsList[index]->m_ID;
    }

    void PresetKeywordsModel::addOrUpdatePreset(const QString &name, const QStringList &keywords, ID_t &id, bool &isAdded) {
        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        size_t index = 0;
        addOrUpdatePresetUnsafe(name, keywords, index, isAdded);
        id = m_PresetsList[index]->m_ID;
    }

    bool PresetKeywordsModel::setPresetGroup(ID_t presetID, int groupID) {
        LOG_INFO << "preset" << presetID << "; group" << groupID;
        bool success = false;

        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        size_t index = 0;

        if (tryGetPresetIndexByIDUnsafe(presetID, index)) {
            if (m_GroupsModel.isGroupIDValid(groupID)) {
                m_PresetsList[index]->m_GroupID = groupID;
                success = true;
            }
        }

        return success;
    }

    void PresetKeywordsModel::findOrRegisterGroup(const QString &groupName, int &groupID) {
        LOG_INFO << groupName;
        m_GroupsModel.findOrRegisterGroup(groupName, groupID);
    }

    void PresetKeywordsModel::requestBackup() {
        LOG_DEBUG << "#";
        emit backupRequest();
    }

    void PresetKeywordsModel::refreshPresets() {
        LOG_DEBUG << "#";
        // used for plugins/background threads
        beginResetModel();
        {
        }
        endResetModel();
    }

    void PresetKeywordsModel::triggerPresetsUpdated() {
        LOG_DEBUG << "#";
        emit presetsUpdated();
    }

    bool PresetKeywordsModel::tryFindPresetByFullName(const QString &name, bool caseSensitive, ID_t &id) {
        bool result = false;

        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);
        size_t index = 0;
        if (tryFindPresetByFullNameUnsafe(name, caseSensitive, index)) {
            id = m_PresetsList[index]->m_ID;
        }

        return result;
    }

    void PresetKeywordsModel::foreachPreset(const std::function<bool (size_t, PresetModel *)> &action) {
        QReadLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        const size_t size = m_PresetsList.size();

        for (size_t i = 0; i < size; i++) {
            PresetModel *preset = m_PresetsList[i];
            const bool shouldContinue = action(i, preset);
            if (!shouldContinue) { break; }
        }
    }

    bool PresetKeywordsModel::tryGetGroupFromIndexUnsafe(size_t index, int &groupID) {
        Q_ASSERT(index < m_PresetsList.size());
        groupID = m_PresetsList[index]->m_GroupID;
        return true;
    }

    bool PresetKeywordsModel::tryGetNameFromIndexUnsafe(size_t index, QString &name) {
        Q_ASSERT(index < m_PresetsList.size());
        name = m_PresetsList[index]->m_PresetName;
        return true;
    }

    bool PresetKeywordsModel::tryGetPresetUnsafe(size_t presetIndex, QStringList &keywords) {
        Q_ASSERT(presetIndex < m_PresetsList.size());
        auto *preset = m_PresetsList[presetIndex];
        auto &keywordsModel = preset->m_KeywordsModel;
        keywords = keywordsModel.getKeywords();
        return true;
    }

    bool PresetKeywordsModel::tryGetPresetIndexByIDUnsafe(ID_t id, size_t &index) {
        const size_t presetIndex = (size_t)id;
        const size_t size = m_PresetsList.size();
        bool found = false;

        if (presetIndex < size) {
            auto *preset = m_PresetsList[presetIndex];
            if (preset->m_ID == id) {
                found = true;
                index = presetIndex;
            }
        }

        if (!found) {
            for (size_t i = 0; i < size; i++) {
                auto *preset = m_PresetsList[i];
                if (preset->m_ID == id) {
                    found = true;
                    index = i;
                    break;
                }
            }
        }

        return found;
    }

    bool PresetKeywordsModel::tryFindSinglePresetByNameUnsafe(const QString &name, bool strictMatch, size_t &index) {
        LOG_INFO << name;
        int foundIndex = -1;
        size_t size = m_PresetsList.size();
        bool anyError = false;

        if (!strictMatch) {
            for (size_t i = 0; i < size; ++i) {
                PresetModel *preset = m_PresetsList[i];

                if (QString::compare(name, preset->m_PresetName, Qt::CaseInsensitive) == 0) {
                    // full match always overrides
                    foundIndex = (int)i;
                    anyError = false;
                    break;
                } else if (preset->m_PresetName.contains(name, Qt::CaseInsensitive)) {
                    if (foundIndex != -1) {
                        anyError = true;
                        foundIndex = -1;
                        break;
                    } else {
                        foundIndex = (int)i;
                    }
                }
            }
        } else {
            for (size_t i = 0; i < size; ++i) {
                PresetModel *preset = m_PresetsList[i];
                if (preset->m_PresetName == name) {
                    if (foundIndex != -1) {
                        anyError = true;
                        foundIndex = -1;
                        break;
                    } else {
                        foundIndex = (int)i;
                    }
                }
            }
        }

        bool found = !anyError && (foundIndex != -1);

        if (found) {
            index = foundIndex;
            LOG_INFO << "found [" << m_PresetsList[foundIndex]->m_PresetName << "] with index" << foundIndex;
        }

        return found;
    }

    void PresetKeywordsModel::findPresetsByNameUnsafe(const QString &name, QVector<QPair<int, QString> > &results) {
        LOG_INFO << name;
        size_t size = m_PresetsList.size();

        for (size_t i = 0; i < size; ++i) {
            PresetModel *preset = m_PresetsList[i];

            if (preset->m_PresetName.contains(name, Qt::CaseInsensitive)) {
                results.push_back(qMakePair((int)i, preset->m_PresetName));
            }
        }
    }

    void PresetKeywordsModel::findOrRegisterPresetUnsafe(const QString &name, const QStringList &keywords, size_t &index) {
        LOG_INFO << name;

        size_t existingIndex = 0;
        if (!tryFindPresetByFullNameUnsafe(name, false, existingIndex)) {
            const int lastIndex = getPresetsCount();
            const int nextID = generateNextID();

            beginInsertRows(QModelIndex(), lastIndex, lastIndex);
            {
                m_PresetsList.push_back(new PresetModel(nextID, name, keywords, -1));
            }
            endInsertRows();

            index = lastIndex;
        } else {
            index = existingIndex;
        }
    }

    void PresetKeywordsModel::addOrUpdatePresetUnsafe(const QString &name, const QStringList &keywords, size_t &index, bool &isAdded) {
        LOG_INFO << name;

        size_t existingIndex = 0;
        if (!tryFindPresetByFullNameUnsafe(name, false, existingIndex)) {
            const int lastIndex = getPresetsCount();
            const int nextID = generateNextID();

            beginInsertRows(QModelIndex(), lastIndex, lastIndex);
            {
                m_PresetsList.push_back(new PresetModel(nextID, name, keywords, DEFAULT_GROUP_ID));
            }
            endInsertRows();

            index = lastIndex;
            isAdded = true;
        } else {
            PresetModel *preset = m_PresetsList.at(existingIndex);
            Q_ASSERT(preset != nullptr);
            preset->m_KeywordsModel.setKeywords(keywords);

            QModelIndex indexToUpdate = this->index((int)existingIndex);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole << KeywordsStringRole);

            index = existingIndex;
            isAdded = false;
        }
    }

    bool PresetKeywordsModel::tryFindPresetByFullNameUnsafe(const QString &name, bool caseSensitive, size_t &index) {
        LOG_INFO << name;
        int foundIndex = -1;
        size_t size = m_PresetsList.size();
        Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        for (size_t i = 0; i < size; ++i) {
            PresetModel *preset = m_PresetsList[i];

            if (QString::compare(name, preset->m_PresetName, caseSensivity) == 0) {
                // full match always overrides
                foundIndex = (int)i;
                break;
            }
        }

        bool found = foundIndex != -1;

        if (found) {
            index = foundIndex;
            LOG_INFO << "found [" << m_PresetsList[foundIndex]->m_PresetName << "] with index" << foundIndex;
        }

        return found;
    }

    bool PresetKeywordsModel::removeItemUnsafe(size_t index) {
        if (index >= m_PresetsList.size()) { return false; }

        int row = (int)index;

        beginRemoveRows(QModelIndex(), row, row);
        {
            removeInnerItem(row);
        }
        endRemoveRows();

        return true;
    }

    ID_t PresetKeywordsModel::getPresetID(size_t index) {
        Q_ASSERT(index <= m_PresetsList.size());
        return m_PresetsList[index]->m_ID;
    }

    void PresetKeywordsModel::removeItem(int row) {
        if (row < 0 || row >= getPresetsCount()){
            return;
        }

        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        removeItemUnsafe(row);
    }

    void PresetKeywordsModel::addItem() {
        LOG_DEBUG << "#";

        int nextID = generateNextID();
        int lastIndex = -1;

        {
            QWriteLocker locker(&m_PresetsLock);
            Q_UNUSED(locker);

            lastIndex = getPresetsCount();
            m_PresetsList.push_back(new PresetModel(nextID));
        }

        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        {
            // bump
        }
        endInsertRows();
    }

    void PresetKeywordsModel::editKeyword(int index, int keywordIndex, const QString &replacement) {
        LOG_INFO << "item" << index << "keyword" << keywordIndex << "replacement" << replacement;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.editKeyword(keywordIndex, replacement)) {
                xpiks()->submitKeywordForSpellCheck(&keywordsModel, keywordIndex);
                justChanged();
            }
        }
    }

    void PresetKeywordsModel::removeKeywordAt(int index, int keywordIndex) {
        LOG_INFO << "index" << index << "keyword" << keywordIndex;
        QString keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.removeKeywordAt(keywordIndex, keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;
                justChanged();

                QModelIndex indexToUpdate = this->index(index);
                emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
            }
        }
    }

    void PresetKeywordsModel::removeLastKeyword(int index) {
        LOG_INFO << "index" << index;
        QString keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.removeLastKeyword(keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;
                justChanged();

                QModelIndex indexToUpdate = this->index(index);
                emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
            }
        }
    }

    void PresetKeywordsModel::pasteKeywords(int index, const QStringList &keywords) {
        LOG_INFO << "index" << index << "keywords:" << keywords;

        if (0 <= index && index < getPresetsCount()) {
            auto *preset = m_PresetsList.at(index);
            auto &keywordsModel = preset->m_KeywordsModel;
            keywordsModel.appendKeywords(keywords);
            justChanged();
            xpiks()->submitItemForSpellCheck(&keywordsModel);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    void PresetKeywordsModel::plainTextEdit(int index, const QString &rawKeywords, bool spaceIsSeparator) {
        LOG_INFO << "index" << index;

        if (0 <= index && index < getPresetsCount()) {
            auto *preset = m_PresetsList.at(index);
            Common::BasicKeywordsModel &keywordsModel = preset->m_KeywordsModel;

            QVector<QChar> separators;
            separators << QChar(',');
            if (spaceIsSeparator) { separators << QChar(' '); }
            QStringList keywords;
            Helpers::splitKeywords(rawKeywords.trimmed(), separators, keywords);

            keywordsModel.setKeywords(keywords);
            justChanged();
            xpiks()->submitItemForSpellCheck(&keywordsModel);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    void PresetKeywordsModel::appendKeyword(int index, const QString &keyword) {
        LOG_INFO << "index" << index << "keyword" << keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto *preset = m_PresetsList.at(index);
            auto &keywordsModel = preset->m_KeywordsModel;
            keywordsModel.appendKeyword(keyword);
            justChanged();
            xpiks()->submitKeywordForSpellCheck(&keywordsModel, keywordsModel.getKeywordsCount() - 1);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    QObject *PresetKeywordsModel::getKeywordsModel(int index) {
        QObject *result = nullptr;

        if (0 <= index && index < getPresetsCount()) {
            result = &m_PresetsList[index]->m_KeywordsModel;
            QQmlEngine::setObjectOwnership(result, QQmlEngine::CppOwnership);
        }

        return result;
    }

    void PresetKeywordsModel::saveToConfig() {
        LOG_DEBUG << "#";
        {
            QWriteLocker locker(&m_PresetsLock);
            Q_UNUSED(locker);
            m_PresetsConfig.loadFromModel(m_PresetsList, m_GroupsModel.getGroups());
        }
        m_PresetsConfig.sync();
    }

    void PresetKeywordsModel::makeTitleValid(int row) {
        LOG_DEBUG << row;

        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        const QString originalName = m_PresetsList[row]->m_PresetName;
        QString currentName = originalName;
        int attemptNumber = 0;

        while (hasDuplicateNamesUnsafe(currentName, row)) {
            LOG_DEBUG << "Preset" << currentName << "already exists";
            attemptNumber++;
            currentName = QString("%1 (%2)").arg(originalName).arg(attemptNumber);
        }

        if (currentName != originalName) {
            m_PresetsList[row]->m_PresetName = currentName;
            m_PresetsList[row]->setIsNameDuplicateFlag(false);
            justChanged();
            QModelIndex index = this->index(row);
            emit dataChanged(index, index, QVector<int>() << NameRole << IsNameValidRole);
        }
    }

    void PresetKeywordsModel::loadModelFromConfig() {
        beginResetModel();
        {
            doLoadFromConfig();
        }
        endResetModel();

        m_GroupsModel.initGroups(m_PresetsConfig.m_PresetGroupsData);
    }

    void PresetKeywordsModel::onBackupRequested() {
        LOG_DEBUG << "#";
        m_SavingTimer.start(2000);
    }

    void PresetKeywordsModel::onSavingTimerTriggered() {
        LOG_DEBUG << "#";
        saveToConfig();
    }

    void PresetKeywordsModel::doLoadFromConfig() {
        LOG_DEBUG << "#";
        auto &presetData = m_PresetsConfig.m_PresetData;

        // removeAllPresets();

        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        Q_ASSERT(m_PresetsList.empty());

        for (auto &item: presetData) {
            auto &keywords = item.m_Keywords;
            auto &name = item.m_Name;
            size_t index = 0;

            if (!tryFindPresetByFullNameUnsafe(name, false, index)) {
                int nextID = generateNextID();
                PresetModel *model = new PresetModel(nextID, name, keywords, item.m_GroupID);
                m_PresetsList.push_back(model);
                xpiks()->submitItemForSpellCheck(&model->m_KeywordsModel, Common::SpellCheckFlags::Keywords);
            } else {
                LOG_WARNING << "Preset" << name << "already exists. Skipping...";
            }
        }
    }

    void PresetKeywordsModel::removeAllPresets() {
        QWriteLocker locker(&m_PresetsLock);
        Q_UNUSED(locker);

        for (auto *p: m_PresetsList) {
            if (p->release()) {
                delete p;
            } else {
                m_Finalizers.push_back(p);
            }
        }

        m_PresetsList.clear();
    }

    int PresetKeywordsModel::generateNextID() {
        int lastID = m_LastUsedID.fetchAndAddOrdered(1);
        return lastID;
    }

    bool PresetKeywordsModel::hasDuplicateNamesUnsafe(const QString &presetName, size_t index) {
        if (presetName.isEmpty()) { return false; }

        bool anyDuplicate = false;

        const size_t size = m_PresetsList.size();
        for (size_t i = 0; i < size; i++) {
            if (i == index) { continue; }

            if (QString::compare(m_PresetsList[i]->m_PresetName, presetName, Qt::CaseInsensitive) == 0) {
                anyDuplicate = true;
                break;
            }
        }

        return anyDuplicate;
    }

    int PresetKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_PresetsList.size();
    }

    QVariant PresetKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return QVariant();
        }

        auto *item = m_PresetsList[row];

        switch (role) {
        case NameRole:
            return item->m_PresetName;
        case KeywordsCountRole:
            return item->m_KeywordsModel.getKeywordsCount();
        case KeywordsStringRole:
            return item->m_KeywordsModel.getKeywordsString();
        case GroupRole:
            return item->m_GroupID;
        case IdRole:
            return item->m_ID;
        case IsNameValidRole:
            return !item->getIsNameDuplicateFlag();
        default:
            return QVariant();
        }
    }

    bool PresetKeywordsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return false;
        }

        switch (role) {
        case EditNameRole: {
            auto &name = m_PresetsList[row]->m_PresetName;
            auto newName = value.toString();
            QString sanitized = newName.simplified();

            if (name != sanitized) {
                LOG_INFO << "Preset" << row << name << "renamed to" << sanitized;
                m_PresetsList[row]->m_PresetName = sanitized;
                bool isDuplicate = hasDuplicateNamesUnsafe(name, row);
                m_PresetsList[row]->setIsNameDuplicateFlag(isDuplicate);
                justChanged();
                emit dataChanged(index, index, QVector<int>() << NameRole << IsNameValidRole);
                return true;
            }

            break;
        }
        case EditGroupRole: {
            const int groupID = value.toInt();
            if (groupID != m_PresetsList[row]->m_GroupID) {
                LOG_INFO << "Preset" << row << "group changed to" << groupID;
                m_PresetsList[row]->m_GroupID = groupID;
                justChanged();
                emit dataChanged(index, index, QVector<int>() << GroupRole);
                return true;
            }

            break;
        }
        }

        return false;
    }

    Qt::ItemFlags PresetKeywordsModel::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    QHash<int, QByteArray> PresetKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[NameRole] = "name";
        roles[EditNameRole] = "editname";
        roles[KeywordsCountRole] = "keywordscount";
        roles[KeywordsStringRole] = "keywordsstring";
        roles[GroupRole] = "group";
        roles[EditGroupRole] = "editgroup";
        roles[IdRole] = "pid";
        roles[IsNameValidRole] = "isnamevalid";
        return roles;
    }

    void PresetKeywordsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < getPresetsCount());
        PresetModel *item = m_PresetsList[row];
        m_PresetsList.erase(m_PresetsList.begin() + row);
        if (item->release()) {
            delete item;
        } else {
            m_Finalizers.push_back(item);
        }
    }

    int FilteredPresetsModelBase::getOriginalIndex(int index) {
        LOG_INFO << index;
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    unsigned int FilteredPresetsModelBase::getOriginalID(int index) {
        LOG_INFO << index;
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        PresetKeywordsModel *presetsModel = getPresetsModel();
        ID_t id = presetsModel->getPresetID(row);
        return (unsigned int)id;
    }

    QString FilteredPresetsModelBase::getName(int index) {
        int originalIndex = getOriginalIndex(index);
        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;

        if (!presetsModel->tryGetNameFromIndex(originalIndex, name)) {
            name = QObject::tr("Untitled");
        }

        return name;
    }

    PresetKeywordsModel *FilteredPresetsModelBase::getPresetsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        PresetKeywordsModel *presetsModel = dynamic_cast<PresetKeywordsModel *>(sourceItemModel);
        Q_ASSERT(presetsModel != nullptr);
        return presetsModel;
    }

    void FilteredPresetKeywordsModel::setSearchTerm(const QString &value) {
        LOG_INFO << value;

        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
    }

    bool FilteredPresetKeywordsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        if (m_SearchTerm.trimmed().isEmpty()) {
            return true;
        }

        bool result = false;

        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;
        if (presetsModel->tryGetNameFromIndexUnsafe(sourceRow, name)) {
            result = name.contains(m_SearchTerm, Qt::CaseInsensitive);
        }

        return result;
    }

    PresetKeywordsGroupModel::PresetKeywordsGroupModel(int groupID):
        m_GroupID(groupID)
    {
        LOG_DEBUG << "Instantiated for group" << groupID;
    }

    bool PresetKeywordsGroupModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        bool result = false;

        PresetKeywordsModel *presetsModel = getPresetsModel();
        int groupID;
        if (presetsModel->tryGetGroupFromIndexUnsafe(sourceRow, groupID)) {
            result = m_GroupID == groupID;
        }

        return result;
    }
}
