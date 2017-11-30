/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BASICKEYWORDSMODELIMPL_H
#define BASICKEYWORDSMODELIMPL_H

#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QHash>
#include <QSet>
#include <QVector>
#include <QHash>
#include <QReadWriteLock>
#include <vector>
#include "baseentity.h"
#include "hold.h"
#include "keyword.h"
#include "flags.h"
#include "imetadataoperator.h"
#include "wordanalysisresult.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModel;

    class BasicKeywordsModelImpl
    {
        friend class BasicKeywordsModel;

    public:
        BasicKeywordsModelImpl(Common::Hold &hold);
        virtual ~BasicKeywordsModelImpl() { }

    public:
        int getKeywordsCount();
        QSet<QString> getKeywordsSet();
        virtual QString getKeywordsString();

    public:
        bool hasKeywords(const QStringList &keywordsList);
        bool areKeywordsEmpty();

    private:
        inline size_t getKeywordsSizeUnsafe() { return m_KeywordsList.size(); }
        inline Keyword &accessKeywordUnsafe(size_t row) { Q_ASSERT(row < m_KeywordsList.size()); return m_KeywordsList.at(row); }

    private:
        bool appendKeywordUnsafe(const QString &keyword, size_t &index);
        void takeKeywordAtUnsafe(size_t index, QString &removedKeyword, bool &wasCorrect);
        size_t appendKeywordsUnsafe(const QStringList &keywordsList, size_t &addedStartIndex, size_t &addedEndIndex);
        bool canEditKeywordUnsafe(size_t index, const QString &replacement) const;
        bool editKeywordUnsafe(size_t index, const QString &replacement);
        bool replaceKeywordUnsafe(size_t index, const QString &existing, const QString &replacement);
        bool clearKeywordsUnsafe();
        bool containsKeywordUnsafe(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::Keywords);
        bool hasKeywordsSpellErrorUnsafe() const;
        bool hasKeywordsDuplicatesUnsafe() const;
        bool removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove, bool caseSensitive, QVector<int> &indicesToRemove);
        bool hasKeywordsUnsafe(const QStringList &keywordsList) const;
        QStringList generateStringListUnsafe();

    public:
        QReadWriteLock *accessLock() { return &m_KeywordsLock; }
        void lockKeywordsRead() { m_KeywordsLock.lockForRead(); }
        void lockKeywordsWrite() { m_KeywordsLock.lockForWrite(); }
        void unlockKeywords() { m_KeywordsLock.unlock(); }

    public:
        QString retrieveKeyword(size_t wordIndex);
        QStringList getKeywords();
        void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        std::vector<KeywordItem> retrieveMisspelledKeywords();
        std::vector<KeywordItem> retrieveDuplicatedKeywords();
        Common::KeywordReplaceResult fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement);
        bool processFailedKeywordReplacementsUnsafe(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval,
                                              QVector<int> &indicesToRemove);

    private:
        bool replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                     Common::SearchFlags flags,
                                     QVector<int> &indicesToRemove,
                                     QVector<int> &indicesToUpdate);

    public:
        bool containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::ExactKeywords);
        bool containsKeywords(const QStringList &keywordsList);
        bool isEmpty();
        bool hasKeywordsSpellError();
        bool hasKeywordsDuplicates();
        bool hasSpellErrors();
        bool hasDuplicates();

    public:
        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

    private:
        void resetSpellCheckResultsUnsafe();
        bool canBeAddedUnsafe(const QString &keyword) const;

    public:
        bool hasKeyword(const QString &keyword);
        bool canEditKeyword(int index, const QString &replacement);

    private:
        void setSpellCheckResultsUnsafe(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        bool isReplacedADuplicateUnsafe(size_t index, const QString &existingPrev,
                                        const QString &replacement) const;

    private:
        Common::Hold &m_Hold;
        QReadWriteLock m_KeywordsLock;
        std::vector<Keyword> m_KeywordsList;
        QSet<QString> m_KeywordsSet;
    };
}

#endif // BASICKEYWORDSMODELIMPL_H
