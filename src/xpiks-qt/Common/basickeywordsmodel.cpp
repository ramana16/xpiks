/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "basickeywordsmodel.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordshelpers.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(Hold &hold, QObject *parent):
        AbstractListModel(parent),
        m_Hold(hold)
    {}

    void BasicKeywordsModel::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        LOG_INFO << "#";

        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        int rangesLength = Helpers::getRangesLength(ranges);
        AbstractListModel::doRemoveItemsAtIndices(ranges, rangesLength);
    }

    void BasicKeywordsModel::removeInnerItem(int row) {
        QString removedKeyword;
        bool wasCorrect = false;

        this->takeKeywordAtUnsafe(row, removedKeyword, wasCorrect);
        LOG_INTEGRATION_TESTS << "keyword:" << removedKeyword << "was correct:" << wasCorrect;
        Q_UNUSED(removedKeyword);
        Q_UNUSED(wasCorrect);
    }

    int BasicKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);

        // QReadLocker readLocker(&m_KeywordsLock);
        // Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there
        // for now we only change number of keywords from the UI anyways

        return (int)m_KeywordsList.size();
    }

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        // QReadLocker readLocker(&m_KeywordsLock);
        // Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there
        // for now we only change number of keywords from the UI anyways

        const int row = index.row();

        if (row < 0 || row >= (int)m_KeywordsList.size()) {
            return QVariant();
        }

        auto &keyword = m_KeywordsList.at(row);

        switch (role) {
            case KeywordRole:
                return keyword.m_Value;
            case IsCorrectRole:
                return keyword.m_IsCorrect;
            case HasDuplicateRole:
                return keyword.m_HasDuplicates;
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> BasicKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[IsCorrectRole] = "iscorrect";
        roles[HasDuplicateRole] = "hasduplicate";
        return roles;
    }

    int BasicKeywordsModel::getKeywordsCount() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return (int)m_KeywordsSet.size();
    }

    QSet<QString> BasicKeywordsModel::getKeywordsSet() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsSet;
    }

    QString BasicKeywordsModel::getKeywordsString() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        QStringList keywords = generateStringListUnsafe();
        QString result = keywords.join(", ");
        return result;
    }

    bool BasicKeywordsModel::appendKeyword(const QString &keyword) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        return appendKeywordUnsafe(keyword);
    }

    bool BasicKeywordsModel::removeKeywordAt(size_t index, QString &removedKeyword) {
        bool wasCorrect = false, removed = false;

        m_KeywordsLock.lockForWrite();
        {
            if (index < m_KeywordsList.size()) {
                beginRemoveRows(QModelIndex(), (int)index, (int)index);
                {
                    takeKeywordAtUnsafe(index, removedKeyword, wasCorrect);
                }
                endRemoveRows();
                removed = true;
            }
        }
        m_KeywordsLock.unlock();

        if (removed) {
            if (!wasCorrect) {
                emit spellCheckErrorsChanged();
            }

            emit hasDuplicatesChanged();
        }

        return removed;
    }

    bool BasicKeywordsModel::removeLastKeyword(QString &removedKeyword) {
        bool wasCorrect = false, removed = false;

        m_KeywordsLock.lockForWrite();
        {
            const size_t size = m_KeywordsList.size();
            if (size > 0) {
                const size_t indexLast = size - 1;
                beginRemoveRows(QModelIndex(), (int)indexLast, (int)indexLast);
                {
                    takeKeywordAtUnsafe(indexLast, removedKeyword, wasCorrect);
                }
                endRemoveRows();
                removed = true;
            }
        }
        m_KeywordsLock.unlock();

        if (removed) {
            if (!wasCorrect) {
                emit spellCheckErrorsChanged();
            }

            emit hasDuplicatesChanged();
        }

        return removed;
    }

    void BasicKeywordsModel::setKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        setKeywordsUnsafe(keywordsList);
    }

    size_t BasicKeywordsModel::appendKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);
        return appendKeywordsUnsafe(keywordsList);
    }

    bool BasicKeywordsModel::editKeyword(size_t index, const QString &replacement) {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (index < m_KeywordsList.size()) {
                result = editKeywordUnsafe(index, replacement);
            } else {
                LOG_WARNING << "Failed to edit keyword with index" << index;
            }
        }
        m_KeywordsLock.unlock();

        if (result) {
            QModelIndex i = this->index((int)index);
            emit dataChanged(i, i, QVector<int>() << KeywordRole);
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywords() {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            result = clearKeywordsUnsafe();
        }
        m_KeywordsLock.unlock();

        if (result) {
            emit spellCheckErrorsChanged();
            emit hasDuplicatesChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::expandPreset(size_t keywordIndex, const QStringList &presetList) {
        LOG_INFO << keywordIndex;
        bool result = false;

        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        if (keywordIndex < m_KeywordsList.size()) {
            expandPresetUnsafe(keywordIndex, presetList);
            result = true;
        }

        return result;
    }

    bool BasicKeywordsModel::appendPreset(const QStringList &presetList) {
        LOG_DEBUG << "#";
        bool result = false;
        result = appendKeywords(presetList) > 0;
        return result;
    }

    bool BasicKeywordsModel::hasKeywords(const QStringList &keywordsList) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        bool result = hasKeywordsUnsafe(keywordsList);
        return result;
    }

    bool BasicKeywordsModel::areKeywordsEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsList.empty();
    }

    bool BasicKeywordsModel::replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) {
        LOG_INTEGR_TESTS_OR_DEBUG << replaceWhat << "->" << replaceTo << "with flags:" << (int)flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(((int)flags & (int)Common::SearchFlags::Metadata) != 0);

        bool anyChanged = false;

        const bool needToCheckKeywords = Common::HasFlag(flags, Common::SearchFlags::Keywords);
        if (needToCheckKeywords) {
            QWriteLocker locker(&m_KeywordsLock);
            Q_UNUSED(locker);
            if (this->replaceInKeywordsUnsafe(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::removeKeywords(const QSet<QString> &keywords, bool caseSensitive) {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            result = removeKeywordsUnsafe(keywords, caseSensitive);
        }
        m_KeywordsLock.unlock();

        return result;
    }

    bool BasicKeywordsModel::appendKeywordUnsafe(const QString &keyword) {
        bool added = false;
        QString sanitizedKeyword = keyword.simplified();

        if (canBeAddedUnsafe(sanitizedKeyword)) {
            size_t keywordsCount = m_KeywordsList.size();

            m_KeywordsSet.insert(sanitizedKeyword.toLower());

            beginInsertRows(QModelIndex(), (int)keywordsCount, (int)keywordsCount);
            {
                m_KeywordsList.emplace_back(sanitizedKeyword);
            }
            endInsertRows();
            added = true;
        }

        return added;
    }

    void BasicKeywordsModel::takeKeywordAtUnsafe(size_t index, QString &removedKeyword, bool &wasCorrect) {
        Q_ASSERT((0 <= index) && (index < m_KeywordsList.size()));
        const auto &keyword = m_KeywordsList.at(index);
        const QString invariant = keyword.m_Value.toLower();

        m_KeywordsSet.remove(invariant);

        wasCorrect = keyword.m_IsCorrect;
        removedKeyword = keyword.m_Value;

        m_KeywordsList.erase(m_KeywordsList.begin() + index);
    }

    void BasicKeywordsModel::setKeywordsUnsafe(const QStringList &keywordsList) {
        clearKeywordsUnsafe();
        appendKeywordsUnsafe(keywordsList);
    }

    size_t BasicKeywordsModel::appendKeywordsUnsafe(const QStringList &keywordsList) {
        QStringList keywordsToAdd;
        int appendedCount = 0, size = keywordsList.length();

        keywordsToAdd.reserve(size);
        QSet<QString> accountedKeywords;

        for (int i = 0; i < size; ++i) {
            const QString &keyword = keywordsList.at(i);
            const QString &sanitizedKeyword = keyword.simplified();
            const QString &lowerCased = sanitizedKeyword.toLower();

            if (canBeAddedUnsafe(sanitizedKeyword) && !accountedKeywords.contains(lowerCased)) {
                keywordsToAdd.append(sanitizedKeyword);
                accountedKeywords.insert(lowerCased);
                appendedCount++;
            }
        }

        size = keywordsToAdd.size();
        Q_ASSERT(size == appendedCount);

        if (size > 0) {
            int rowsCount = (int)m_KeywordsList.size();
            beginInsertRows(QModelIndex(), rowsCount, rowsCount + size - 1);

            for (int i = 0; i < size; ++i) {
                const QString &keywordToAdd = keywordsToAdd.at(i);
                m_KeywordsSet.insert(keywordToAdd.toLower());
                m_KeywordsList.emplace_back(keywordToAdd);
            }

            endInsertRows();
        }

        return appendedCount;
    }

    bool BasicKeywordsModel::canEditKeywordUnsafe(size_t index, const QString &replacement) const {
        Q_ASSERT((0 <= index) && (index < m_KeywordsList.size()));
        bool result = false;
        LOG_INFO << "index:" << index << "replacement:" << replacement;

        QString sanitized = Helpers::doSanitizeKeyword(replacement);
        const QString existing = m_KeywordsList.at(index).m_Value;
        // IMPORTANT: keep track of copy-paste in editKeywordUnsafe()
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            QString lowerCasedNew = sanitized.toLower();
            QString lowerCasedExisting = existing.toLower();

            if (!m_KeywordsSet.contains(lowerCasedNew)) {
                result = true;
            } else if (lowerCasedNew == lowerCasedExisting) {
                result = true;
            }
        }

        return result;
    }

    bool BasicKeywordsModel::editKeywordUnsafe(size_t index, const QString &replacement) {
        Q_ASSERT((0 <= index) && (index < m_KeywordsList.size()));
        bool result = false;

        LOG_INFO << "index:" << index << "replacement:" << replacement;
        QString sanitized = Helpers::doSanitizeKeyword(replacement);

        auto &keyword = m_KeywordsList.at(index);
        QString existing = keyword.m_Value;
        // IMPORTANT: keep track of copy-paste in canEditKeywordUnsafe()
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            QString lowerCasedNew = sanitized.toLower();
            QString lowerCasedExisting = existing.toLower();

            if (!m_KeywordsSet.contains(lowerCasedNew)) {
                m_KeywordsSet.insert(lowerCasedNew);
                m_KeywordsList[index].m_Value = sanitized;
                m_KeywordsSet.remove(lowerCasedExisting);
                LOG_INFO << "common case edit:" << existing << "->" << sanitized;

                result = true;
            } else if (lowerCasedNew == lowerCasedExisting) {
                LOG_INFO << "changing case in same keyword";
                m_KeywordsList[index].m_Value = sanitized;

                result = true;
            } else {
                LOG_WARNING << "Attempt to rename keyword to existing one. Use remove instead!";
            }
        }

        return result;
    }

    bool BasicKeywordsModel::replaceKeywordUnsafe(size_t index, const QString &existing, const QString &replacement) {
        Q_ASSERT((0 <= index) && (index < m_KeywordsList.size()));
        bool result = false;

        auto &keyword = m_KeywordsList.at(index);
        const QString &internal = keyword.m_Value;

        if (internal == existing) {
            if (this->editKeywordUnsafe(index, replacement)) {
                result = true;
            }
        } else if (internal.contains(existing) && internal.contains(QChar::Space)) {
            LOG_INFO << "Replacing composite keyword";
            QString existingFixed = internal;
            existingFixed.replace(existing, replacement);
            if (this->editKeywordUnsafe(index, existingFixed)) {
                // TODO: reimplement this someday
                // no need to mark keyword as correct
                // if we replace only part of it
                result = true;
            }
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywordsUnsafe() {
        const bool anyKeywords = !m_KeywordsList.empty();

        if (anyKeywords) {
            beginResetModel();
            {
                m_KeywordsList.clear();
            }
            endResetModel();

            m_KeywordsSet.clear();
        } else {
            Q_ASSERT(m_KeywordsSet.empty());
        }

        return anyKeywords;
    }

    bool BasicKeywordsModel::containsKeywordUnsafe(const QString &searchTerm, Common::SearchFlags searchFlags) {
        bool hasMatch = false;
        const bool caseSensitive = Common::HasFlag(searchFlags, Common::SearchFlags::CaseSensitive);
        Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        const bool wholeWords = Common::HasFlag(searchFlags, Common::SearchFlags::WholeWords);

        if (wholeWords) {
            for (auto &keyword: m_KeywordsList) {
                if (QString::compare(keyword.m_Value, searchTerm, caseSensivity) == 0) {
                    hasMatch = true;
                    break;
                }
            }
        } else {
            for (auto &keyword: m_KeywordsList) {
                if (keyword.m_Value.contains(searchTerm, caseSensivity)) {
                    hasMatch = true;
                    break;
                }
            }
        }

        return hasMatch;
    }

    bool BasicKeywordsModel::hasKeywordsSpellErrorUnsafe() const {
        bool anyError = false;

        for (auto &keyword: m_KeywordsList) {
            if (!keyword.m_IsCorrect) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasKeywordsDuplicatesUnsafe() const {
        bool hasDuplicate = false;

        for (auto &keyword: m_KeywordsList) {
            if (keyword.m_HasDuplicates) {
                hasDuplicate = true;
                break;
            }
        }

        return hasDuplicate;
    }

    bool BasicKeywordsModel::removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove, bool caseSensitive) {
        size_t size = m_KeywordsList.size();

        QVector<int> indicesToRemove;
        indicesToRemove.reserve((int)size/2);

        for (size_t i = 0; i < size; ++i) {
            QString keyword = m_KeywordsList.at(i).m_Value;

            if (!caseSensitive) {
                keyword = keyword.toLower();
            }

            if (keywordsToRemove.contains(keyword)) {
                indicesToRemove.append((int)i);
            }
        }

        bool anythingRemoved = !indicesToRemove.empty();
        removeKeywordsAtIndicesUnsafe(indicesToRemove);

        return anythingRemoved;
    }

    void BasicKeywordsModel::expandPresetUnsafe(size_t keywordsIndex, const QStringList &keywordsList) {
        Q_ASSERT((0 <= keywordsIndex) && (keywordsIndex < m_KeywordsList.size()));

        LOG_INFO << "index" << keywordsIndex << "list:" << keywordsList;
        QString removedKeyword;
        bool wasCorrect = false;

        beginRemoveRows(QModelIndex(), (int)keywordsIndex, (int)keywordsIndex);
        {
            this->takeKeywordAtUnsafe(keywordsIndex, removedKeyword, wasCorrect);
        }
        endRemoveRows();
        LOG_INFO << "replaced keyword" << removedKeyword;
        Q_UNUSED(wasCorrect);

        size_t addedCount = appendKeywordsUnsafe(keywordsList);
        LOG_INFO << addedCount << "new added";
    }

    bool BasicKeywordsModel::hasKeywordsUnsafe(const QStringList &keywordsList) const {
        bool anyMissing = false;

        for (auto &item: keywordsList) {
            if (canBeAddedUnsafe(item.simplified())) {
                anyMissing = true;
                break;
            }
        }

        return !anyMissing;
    }

    QStringList BasicKeywordsModel::generateStringListUnsafe() {
        QStringList result;
        result.reserve((int)m_KeywordsList.size());

        for (auto &keyword: m_KeywordsList) {
            result.append(keyword.m_Value);
        }

        return result;
    }

    QString BasicKeywordsModel::retrieveKeyword(size_t wordIndex) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        QString keyword;
        if (wordIndex < m_KeywordsList.size()) {
            keyword = m_KeywordsList.at(wordIndex).m_Value;
        }

        return keyword;
    }

    QStringList BasicKeywordsModel::getKeywords() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return generateStringListUnsafe();
    }

    void BasicKeywordsModel::setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
        LOG_DEBUG << items.size() << "results";

        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        setSpellCheckResultsUnsafe(items);
    }

    std::vector<KeywordItem> BasicKeywordsModel::retrieveMisspelledKeywords() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        std::vector<KeywordItem> misspelledKeywords;
        size_t size = m_KeywordsList.size();
        misspelledKeywords.reserve(size/3);

        for (size_t i = 0; i < size; ++i) {
            auto &item = m_KeywordsList.at(i);
            if (!item.m_IsCorrect) {
                const QString &keyword = item.m_Value;
                LOG_INTEGR_TESTS_OR_DEBUG << keyword << "has wrong spelling";

                if (!keyword.contains(QChar::Space)) {
                    misspelledKeywords.emplace_back(keyword, i);
                } else {
                    QStringList items = keyword.split(QChar::Space, QString::SkipEmptyParts);
                    foreach(const QString &item, items) {
                        misspelledKeywords.emplace_back(item, i, keyword);
                    }
                }
            }
        }

        return misspelledKeywords;
    }

    std::vector<KeywordItem> BasicKeywordsModel::retrieveDuplicatedKeywords() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        std::vector<KeywordItem> duplicatedKeywords;
        size_t size = m_KeywordsList.size();
        duplicatedKeywords.reserve(size/3);

        for (size_t i = 0; i < size; ++i) {
            auto &item = m_KeywordsList.at(i);
            if (item.m_HasDuplicates) {
                duplicatedKeywords.emplace_back(item.m_Value, i);
            }
        }

        return duplicatedKeywords;
    }

    Common::KeywordReplaceResult BasicKeywordsModel::fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement) {
        Common::KeywordReplaceResult result;

        LOG_INFO << "Replacing" << existing << "to" << replacement << "with index" << index;

        m_KeywordsLock.lockForWrite();
        {
            const size_t size = m_KeywordsList.size();
            if (index < size) {
                if (replaceKeywordUnsafe(index, existing, replacement)) {
                    m_KeywordsList[index].m_IsCorrect = true;
                    result = Common::KeywordReplaceResult::Succeeded;
                } else {
                    result = Common::KeywordReplaceResult::FailedDuplicate;
                }
            } else {
                LOG_INFO << "Failure. Index is negative or exceeds count" << size;
                result = Common::KeywordReplaceResult::FailedIndex;
            }
        }
        m_KeywordsLock.unlock();

        if (result == Common::KeywordReplaceResult::Succeeded) {
            QModelIndex i = this->index((int)index);
            // combined roles from legacy editKeyword() and replace()
            emit dataChanged(i, i, QVector<int>() << KeywordRole << IsCorrectRole << HasDuplicateRole);
        }

        return result;
    }

    bool BasicKeywordsModel::processFailedKeywordReplacements(const
                                                              std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval)
    {
        LOG_INFO << candidatesForRemoval.size() << "candidates to remove";
        bool anyReplaced = false;

        if (candidatesForRemoval.empty()) {
            return anyReplaced;
        }

        QVector<int> indicesToRemove;
        size_t size = candidatesForRemoval.size();
        indicesToRemove.reserve((int)size);

        QWriteLocker writeLocker(&m_KeywordsLock);

        for (size_t i = 0; i < size; ++i) {
            auto &item = candidatesForRemoval.at(i);

            size_t index = item->getOriginalIndex();
            if (index >= m_KeywordsList.size()) {
                LOG_DEBUG << "index is out of range";
                continue;
            }

            const QString &existingPrev = item->getWord();
            QString sanitized = Helpers::doSanitizeKeyword(item->getReplacement());

            if (isReplacedADuplicateUnsafe(index, existingPrev, sanitized)) {
                indicesToRemove.append((int)index);
            }
        }

        LOG_INFO << "confirmed" << indicesToRemove.size() << "duplicates to remove";

        if (!indicesToRemove.isEmpty()) {
            this->removeKeywordsAtIndicesUnsafe(indicesToRemove);
            anyReplaced = true;
        }

        return anyReplaced;
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, &SpellCheck::SpellCheckItem::resultsReady,
                         this, &BasicKeywordsModel::onSpellCheckRequestReady);
    }

    void BasicKeywordsModel::afterReplaceCallback() {
        LOG_DEBUG << "#";
        emit spellCheckErrorsChanged();
        emit afterSpellingErrorsFixed();
        emit hasDuplicatesChanged();
    }

    void BasicKeywordsModel::removeKeywordsAtIndicesUnsafe(const QVector<int> &indices) {
        LOG_DEBUG << indices.size() << "item(s)";
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indices, rangesToRemove);
        AbstractListModel::removeItemsAtIndices(rangesToRemove);
    }

    bool BasicKeywordsModel::replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                                     Common::SearchFlags flags) {
        bool anyChanged = false;

        QVector<int> indicesToRemove;

        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::WholeWords);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        const size_t size = m_KeywordsList.size();
        for (size_t i = 0; i < size; ++i) {
            auto &keyword = m_KeywordsList.at(i);
            QString internal = keyword.m_Value;
            const bool hasMatch = wholeWords ?
                                  Helpers::containsWholeWords(internal, replaceWhat, caseSensivity) :
                                  internal.contains(replaceWhat, caseSensivity);
            LOG_FOR_TESTS << "[" << internal << "] has match [" << replaceWhat << "] =" << hasMatch;

            if (hasMatch) {
                QString replaced = wholeWords ?
                                   Helpers::replaceWholeWords(internal, replaceWhat, replaceTo, caseSensivity) :
                                   internal.replace(replaceWhat, replaceTo, caseSensivity);

                QString replacement = Helpers::doSanitizeKeyword(replaced);

                if (!this->editKeywordUnsafe(i, replacement)) {
                    if (replacement.isEmpty()) {
                        LOG_INFO << "Replaced" << internal << "to empty";
                        indicesToRemove.append((int)i);
                    } else if (m_KeywordsSet.contains(replacement.toLower())) {
                        LOG_INFO << "Replacing" << internal << "to" << replacement << "creates a duplicate";
                        indicesToRemove.append((int)i);
                    }
                } else {
                    QModelIndex j = this->index((int)i);
                    emit dataChanged(j, j, QVector<int>() << KeywordRole);
                    anyChanged = true;
                }
            }
        }

        if (!indicesToRemove.isEmpty()) {
            this->removeKeywordsAtIndicesUnsafe(indicesToRemove);
            anyChanged = true;
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return containsKeywordUnsafe(searchTerm, searchFlags);
    }

    bool BasicKeywordsModel::containsKeywords(const QStringList &keywordsList) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        bool anyError = false;

        Common::SearchFlags searchFlags = Common::SearchFlags::ExactKeywords;
        for (auto &keyword: keywordsList) {
            if (!containsKeywordUnsafe(keyword, searchFlags)) {
                anyError = true;
                break;
            }
        }

        return !anyError;
    }

    bool BasicKeywordsModel::isEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsList.empty();
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return hasKeywordsSpellErrorUnsafe();
    }

    bool BasicKeywordsModel::hasKeywordsDuplicates() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return hasKeywordsDuplicatesUnsafe();
    }

    bool BasicKeywordsModel::hasSpellErrors() {
        bool hasErrors = hasKeywordsSpellError();
        return hasErrors;
    }

    bool BasicKeywordsModel::hasDuplicates() {
        bool hasDuplicates = hasKeywordsDuplicates();
        return hasDuplicates;
    }

    void BasicKeywordsModel::notifySpellCheckResults(Common::SpellCheckFlags flags) {
        LOG_INTEGR_TESTS_OR_DEBUG << (int)flags;

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Description) ||
            Common::HasFlag(flags, Common::SpellCheckFlags::Title)) {
            emit spellCheckResultsReady();
        }

        emit spellCheckErrorsChanged();
        emit hasDuplicatesChanged();
    }

    void BasicKeywordsModel::resetSpellCheckResultsUnsafe() {
        for (auto &keyword: m_KeywordsList) {
            keyword.m_IsCorrect = true;
        }
    }

    bool BasicKeywordsModel::canBeAddedUnsafe(const QString &keyword) const {
        bool isValid = Helpers::isValidKeyword(keyword);
        bool result = isValid && !m_KeywordsSet.contains(keyword.toLower());

        return result;
    }

    bool BasicKeywordsModel::hasKeyword(const QString &keyword) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return !canBeAddedUnsafe(keyword.simplified());
    }

    bool BasicKeywordsModel::canEditKeyword(int index, const QString &replacement) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return canEditKeywordUnsafe(index, replacement);
    }

    void BasicKeywordsModel::onSpellCheckRequestReady(Common::SpellCheckFlags flags, size_t index) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Keywords)) {
            emitSpellCheckChanged((int)index);
        }

        notifySpellCheckResults(flags);
    }

    void BasicKeywordsModel::setSpellCheckResultsUnsafe(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
        const size_t itemsSize = items.size();
        const size_t keywordsSize = m_KeywordsList.size();

        // reset relative items
        for (size_t i = 0; i < itemsSize; ++i) {
            auto &item = items.at(i);
            size_t index = item->m_Index;
            if (index < keywordsSize) {
                m_KeywordsList[index].resetSpelling();
            }

            if (index >= keywordsSize) {
                LOG_DEBUG << "Skipping the rest of overflowing results";
#ifdef QT_DEBUG
                // if any of these has overflowing index, then all of them should
                for (size_t j = i; j < itemsSize; ++j) {
                    Q_ASSERT(items.at(j)->m_Index >= keywordsSize);
                }
#endif
                break;
            }
        }

        for (size_t i = 0; i < itemsSize; ++i) {
            auto &item = items.at(i);
            size_t index = item->m_Index;
            // looks like this is a stupid assert to trace impossible race conditions
            Q_ASSERT(keywordsSize == m_KeywordsList.size());

            if (index < keywordsSize) {
                auto &keyword = m_KeywordsList[index];
                if (keyword.m_Value.contains(item->m_Word)) {
                    // if keyword contains several words, there would be
                    // several queryiIems and there's error if any has error
                    keyword.m_IsCorrect = keyword.m_IsCorrect && item->m_IsCorrect;
                    keyword.m_HasDuplicates = keyword.m_HasDuplicates || item->m_IsDuplicate;
                }
            }

            if (index >= keywordsSize) {
                LOG_DEBUG << "Skipping the rest of overflowing results";
                break;
            }
        }
    }

    bool BasicKeywordsModel::isReplacedADuplicateUnsafe(size_t index, const QString &existingPrev,
                                                        const QString &replacement) const {
        bool isDuplicate = false;
        const QString &existingCurrent = m_KeywordsList.at(index).m_Value;

        if (existingCurrent == existingPrev) {
            if (m_KeywordsSet.contains(replacement.toLower())) {
                isDuplicate = true;
                LOG_INFO << "safe to remove duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_INFO << replacement << "was not found";
            }
        } else if (existingCurrent.contains(existingPrev) && existingCurrent.contains(QChar::Space)) {
            QString existingFixed = existingCurrent;
            existingFixed.replace(existingPrev, replacement);

            if (m_KeywordsSet.contains(existingFixed.toLower())) {
                isDuplicate = true;
                LOG_INFO << "safe to remove composite duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_INFO << existingFixed << "was not found";
            }
        } else {
            LOG_INFO << existingCurrent << "is now instead of" << existingPrev << "at index" << index;
        }

        return isDuplicate;
    }

    void BasicKeywordsModel::emitSpellCheckChanged(int index) {
        const int count = (int)m_KeywordsList.size();

        if (index == -1) {
            if (count > 0) {
                QModelIndex start = this->index(0);
                QModelIndex end = this->index(count - 1);
                emit dataChanged(start, end, QVector<int>() << IsCorrectRole << HasDuplicateRole);
            }
        } else {
            if (0 <= index && index < count) {
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole << HasDuplicateRole);
            }
        }
    }
}
