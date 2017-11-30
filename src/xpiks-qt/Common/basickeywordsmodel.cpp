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
#include "basickeywordsmodelimpl.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(Hold &hold, QObject *parent):
        AbstractListModel(parent),
        m_Impl(new BasicKeywordsModelImpl(hold))
    {}

#ifdef CORE_TESTS
    const QString &BasicKeywordsModel::getKeywordAt(int index) const { return m_Impl->accessKeywordUnsafe(index).m_Value; }
    std::vector<Keyword> &BasicKeywordsModel::getRawKeywords() { return m_Impl->m_KeywordsList; }
#endif

#ifdef INTEGRATION_TESTS
        bool BasicKeywordsModel::hasDuplicateAt(size_t i) const { return m_Impl->accessKeywordUnsafe(i).m_HasDuplicate; }
#endif

    void BasicKeywordsModel::removeItemsFromRanges(const QVector<QPair<int, int> > &ranges) {
        LOG_INFO << "#";
        int rangesLength = Helpers::getRangesLength(ranges);

        {
            QWriteLocker writeLocker(m_Impl->accessLock());
            Q_UNUSED(writeLocker);

            AbstractListModel::doRemoveItemsFromRanges(ranges);
        }

        AbstractListModel::doEmitItemsRemovedAtIndices(ranges, rangesLength);
    }

    void BasicKeywordsModel::removeInnerItem(int row) {
        QString removedKeyword;
        bool wasCorrect = false;

        m_Impl->takeKeywordAtUnsafe(row, removedKeyword, wasCorrect);
        LOG_INTEGRATION_TESTS << "keyword:" << removedKeyword << "was correct:" << wasCorrect;
        Q_UNUSED(removedKeyword);
        Q_UNUSED(wasCorrect);
    }

    int BasicKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Impl->getKeywordsCount();
    }

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        QReadLocker readLocker(m_Impl->accessLock());
        Q_UNUSED(readLocker);

        const int row = index.row();

        if (row < 0 || row >= (int)m_Impl->getKeywordsSizeUnsafe()) {
            return QVariant();
        }

        auto &keyword = m_Impl->accessKeywordUnsafe(row);

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
        return m_Impl->getKeywordsCount();
    }

    QSet<QString> BasicKeywordsModel::getKeywordsSet() {
        return m_Impl->getKeywordsSet();
    }

    QString BasicKeywordsModel::getKeywordsString() {
        return m_Impl->getKeywordsString();
    }

    bool BasicKeywordsModel::appendKeyword(const QString &keyword) {
        bool added = false;
        size_t index = 0;

        {
            QWriteLocker writeLocker(m_Impl->accessLock());
            Q_UNUSED(writeLocker);
            added = m_Impl->appendKeywordUnsafe(keyword, index);
        }

        if (added) {
            beginInsertRows(QModelIndex(), (int)index, (int)index);
            endInsertRows();
        }

        return added;
    }

    bool BasicKeywordsModel::removeKeywordAt(size_t index, QString &removedKeyword) {
        bool wasCorrect = false, removed = false;

        m_Impl->lockKeywordsWrite();
        {
            if (index < m_Impl->getKeywordsSizeUnsafe()) {
                m_Impl->takeKeywordAtUnsafe(index, removedKeyword, wasCorrect);
                removed = true;
            }
        }
        m_Impl->unlockKeywords();

        if (removed) {
            beginRemoveRows(QModelIndex(), (int)index, (int)index);
            endRemoveRows();

            if (!wasCorrect) {
                notifyKeywordsSpellingChanged();
            }

            emit hasDuplicatesChanged();
        }

        return removed;
    }

    bool BasicKeywordsModel::removeLastKeyword(QString &removedKeyword) {
        bool wasCorrect = false, removed = false;
        size_t indexLast = 0;

        m_Impl->lockKeywordsWrite();
        {
            const size_t size = m_Impl->getKeywordsSizeUnsafe();
            if (size > 0) {
                indexLast = size - 1;
                m_Impl->takeKeywordAtUnsafe(indexLast, removedKeyword, wasCorrect);
                removed = true;
            }
        }
        m_Impl->unlockKeywords();

        if (removed) {
            beginRemoveRows(QModelIndex(), (int)indexLast, (int)indexLast);
            endRemoveRows();

            if (!wasCorrect) {
                notifyKeywordsSpellingChanged();
            }

            emit hasDuplicatesChanged();
        }

        return removed;
    }

    void BasicKeywordsModel::setKeywords(const QStringList &keywordsList) {
        bool anyChange = false;

        {
            QWriteLocker writeLocker(m_Impl->accessLock());
            Q_UNUSED(writeLocker);


            anyChange = m_Impl->clearKeywordsUnsafe();

            size_t start = 0, end = 0;
            size_t size = m_Impl->appendKeywordsUnsafe(keywordsList, start, end);
            if (size > 0) { anyChange = true; }
        }

        if (anyChange) {
            beginResetModel();
            endResetModel();
        }
    }

    size_t BasicKeywordsModel::appendKeywords(const QStringList &keywordsList) {
        size_t size = 0;
        size_t start = 0, end = 0;

        {
            QWriteLocker writeLocker(m_Impl->accessLock());
            Q_UNUSED(writeLocker);
            size = m_Impl->appendKeywordsUnsafe(keywordsList, start, end);
        }

        if (size > 0) {
            beginInsertRows(QModelIndex(), (int)start, (int)end);
            endInsertRows();
        }

        return size;
    }

    bool BasicKeywordsModel::editKeyword(size_t index, const QString &replacement) {
        bool result = false;

        m_Impl->lockKeywordsWrite();
        {
            if (index < m_Impl->getKeywordsSizeUnsafe()) {
                result = m_Impl->editKeywordUnsafe(index, replacement);
            } else {
                LOG_WARNING << "Failed to edit keyword with index" << index;
            }
        }
        m_Impl->unlockKeywords();

        if (result) {
            QModelIndex i = this->index((int)index);
            emit dataChanged(i, i, QVector<int>() << KeywordRole);
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywords() {
        bool result = false;

        m_Impl->lockKeywordsWrite();
        {
            result = m_Impl->clearKeywordsUnsafe();
        }
        m_Impl->unlockKeywords();

        if (result) {
            beginResetModel();
            endResetModel();

            notifyKeywordsSpellingChanged();
            emit hasDuplicatesChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::expandPreset(size_t keywordIndex, const QStringList &presetList) {
        LOG_INFO << keywordIndex;
        bool expanded = false;
        size_t start = 0, end = 0;
        size_t addedCount = 0;

        {
            QWriteLocker writeLocker(m_Impl->accessLock());
            Q_UNUSED(writeLocker);

            if (keywordIndex < m_Impl->getKeywordsSizeUnsafe()) {
                LOG_INFO << "index" << keywordIndex << "list:" << presetList;
                QString removedKeyword;
                bool wasCorrect = false;
                m_Impl->takeKeywordAtUnsafe(keywordIndex, removedKeyword, wasCorrect);

                LOG_INFO << "replaced keyword" << removedKeyword;
                Q_UNUSED(wasCorrect);

                size_t addedCount = m_Impl->appendKeywordsUnsafe(presetList, start, end);
                LOG_INFO << addedCount << "new added";

                expanded = true;
            }
        }

        if (expanded) {
            beginRemoveRows(QModelIndex(), (int)keywordIndex, (int)keywordIndex);
            endRemoveRows();

            if (addedCount > 0) {
                beginInsertRows(QModelIndex(), (int)start, (int)end);
                endInsertRows();
            }
        }

        return expanded;
    }

    bool BasicKeywordsModel::appendPreset(const QStringList &presetList) {
        LOG_DEBUG << "#";
        bool result = appendKeywords(presetList) > 0;
        return result;
    }

    bool BasicKeywordsModel::hasKeywords(const QStringList &keywordsList) {
        return m_Impl->hasKeywords(keywordsList);
    }

    bool BasicKeywordsModel::areKeywordsEmpty() {
        return m_Impl->areKeywordsEmpty();
    }

    bool BasicKeywordsModel::replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) {
        LOG_INTEGR_TESTS_OR_DEBUG << replaceWhat << "->" << replaceTo << "with flags:" << (int)flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(((int)flags & (int)Common::SearchFlags::Metadata) != 0);

        bool anyChanged = false;
        QVector<int> indicesToUpdate, indicesToRemove;
        QVector<QPair<int, int> > rangesToRemove;

        const bool needToCheckKeywords = Common::HasFlag(flags, Common::SearchFlags::Keywords);
        if (needToCheckKeywords) {
            QWriteLocker locker(m_Impl->accessLock());
            Q_UNUSED(locker);

            if (m_Impl->replaceInKeywordsUnsafe(replaceWhat, replaceTo, flags, indicesToRemove, indicesToUpdate)) {
                Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
                AbstractListModel::doRemoveItemsFromRanges(rangesToRemove);
                anyChanged = true;
            }
        }

        if (anyChanged) {
            AbstractListModel::emitRemovedSignalsForRanges(rangesToRemove);
            int size = m_Impl->getKeywordsCount();
            // update all because of easiness instead of calculated correct shifts after removal
            this->updateItemsInRanges({{0, size - 1}}, QVector<int>() << KeywordRole);
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::removeKeywords(const QSet<QString> &keywords, bool caseSensitive) {
        bool anyRemoved = false;

        QVector<QPair<int, int> > rangesToRemove;

        {
            QWriteLocker locker(m_Impl->accessLock());
            Q_UNUSED(locker);

            QVector<int> indicesToRemove;
            if (m_Impl->removeKeywordsUnsafe(keywords, caseSensitive, indicesToRemove)) {
                Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
                AbstractListModel::doRemoveItemsFromRanges(rangesToRemove);
                anyRemoved = true;
            }
        }

        if (anyRemoved) {
            AbstractListModel::emitRemovedSignalsForRanges(rangesToRemove);
        }

        return anyRemoved;
    }

    QString BasicKeywordsModel::retrieveKeyword(size_t wordIndex) {
        return m_Impl->retrieveKeyword(wordIndex);
    }

    QStringList BasicKeywordsModel::getKeywords() {
        return m_Impl->getKeywords();
    }

    void BasicKeywordsModel::setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
       m_Impl->setKeywordsSpellCheckResults(items);
    }

    std::vector<KeywordItem> BasicKeywordsModel::retrieveMisspelledKeywords() {
        return m_Impl->retrieveMisspelledKeywords();
    }

    std::vector<KeywordItem> BasicKeywordsModel::retrieveDuplicatedKeywords() {
        return m_Impl->retrieveDuplicatedKeywords();
    }

    Common::KeywordReplaceResult BasicKeywordsModel::fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement) {
        Common::KeywordReplaceResult result = m_Impl->fixKeywordSpelling(index, existing, replacement);

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
        bool anyReplaced = false;
        QVector<QPair<int, int> > rangesToRemove;

        {
            QWriteLocker locker(m_Impl->accessLock());
            Q_UNUSED(locker);

            QVector<int> indicesToRemove;
            if (m_Impl->processFailedKeywordReplacementsUnsafe(candidatesForRemoval, indicesToRemove)) {
                Q_ASSERT(!indicesToRemove.isEmpty());
                Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
                AbstractListModel::doRemoveItemsFromRanges(rangesToRemove);
                anyReplaced = true;
            }
        }

        AbstractListModel::emitRemovedSignalsForRanges(rangesToRemove);

        return anyReplaced;
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, &SpellCheck::SpellCheckItem::resultsReady,
                         this, &BasicKeywordsModel::onSpellCheckRequestReady);
    }

    void BasicKeywordsModel::afterReplaceCallback() {
        LOG_DEBUG << "#";
        emit notifyKeywordsSpellingChanged();
        emit afterSpellingErrorsFixed();
        emit hasDuplicatesChanged();
    }

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags) {
        return m_Impl->containsKeyword(searchTerm, searchFlags);
    }

    bool BasicKeywordsModel::containsKeywords(const QStringList &keywordsList) {
        return m_Impl->containsKeywords(keywordsList);
    }

    bool BasicKeywordsModel::isEmpty() {
        return m_Impl->isEmpty();
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() {
        return m_Impl->hasKeywordsSpellError();
    }

    bool BasicKeywordsModel::hasKeywordsDuplicates() {
        return m_Impl->hasKeywordsDuplicates();
    }

    bool BasicKeywordsModel::hasSpellErrors() {
        return m_Impl->hasSpellErrors();
    }

    bool BasicKeywordsModel::hasDuplicates() {
        return m_Impl->hasDuplicates();
    }

    void BasicKeywordsModel::notifySpellCheckResults(Common::SpellCheckFlags flags) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Keywords)) {
            notifyKeywordsSpellingChanged();
        }

        emit hasDuplicatesChanged();
    }

    void BasicKeywordsModel::notifyKeywordsSpellingChanged() {
        emit keywordsSpellingChanged();
    }

    void BasicKeywordsModel::acquire() {
        m_Impl->acquire();
    }

    bool BasicKeywordsModel::release() {
        return m_Impl->release();
    }

    bool BasicKeywordsModel::hasKeyword(const QString &keyword) {
        return m_Impl->hasKeyword(keyword);
    }

    bool BasicKeywordsModel::canEditKeyword(int index, const QString &replacement) {
        return m_Impl->canEditKeyword(index, replacement);
    }

    void BasicKeywordsModel::onSpellCheckRequestReady(Common::SpellCheckFlags flags, int index) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Keywords)) {
            updateKeywordsHighlighting(index);
        }

        notifySpellCheckResults(flags);
        emit spellingInfoUpdated();
    }

    void BasicKeywordsModel::updateKeywordsHighlighting(int index) {
        const int count = getKeywordsCount();

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
