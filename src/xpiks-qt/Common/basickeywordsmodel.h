/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BASICKEYWORDSMODEL_H
#define BASICKEYWORDSMODEL_H

#include "abstractlistmodel.h"
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
#include "../Common/flags.h"
#include "../Common/imetadataoperator.h"
#include "../Common/wordanalysisresult.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModel:
            public AbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(bool hasSpellErrors READ hasSpellErrors NOTIFY spellCheckErrorsChanged)
        Q_PROPERTY(bool hasDuplicates READ hasDuplicates NOTIFY hasDuplicatesChanged)

    public:
        BasicKeywordsModel(Common::Hold &hold, QObject *parent=0);

        virtual ~BasicKeywordsModel() {}

    public:
        enum BasicKeywordsModel_Roles {
            KeywordRole = Qt::UserRole + 1,
            IsCorrectRole,
            HasDuplicateRole
        };

#ifdef CORE_TESTS
    public:
        const QString &getKeywordAt(int index) const { return m_KeywordsList.at(index).m_Value; }
        std::vector<Keyword> &getRawKeywords() { return m_KeywordsList; }
#endif

#ifdef INTEGRATION_TESTS
        bool hasDuplicateAt(size_t i) const { return m_KeywordsList.at(i).m_HasDuplicates; }
#endif

    public:
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) override;
        virtual int getRangesLengthForReset() const override { return 10; }

    protected:
        // UNSAFE
        virtual void removeInnerItem(int row) override;

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    public:
        int getKeywordsCount();
        QSet<QString> getKeywordsSet();
        virtual QString getKeywordsString();

    public:
        virtual bool appendKeyword(const QString &keyword);
        virtual bool removeKeywordAt(size_t index, QString &removedKeyword);
        virtual bool removeLastKeyword(QString &removedKeyword);
        virtual void setKeywords(const QStringList &keywordsList);
        virtual int appendKeywords(const QStringList &keywordsList);
        virtual bool editKeyword(size_t index, const QString &replacement);
        virtual bool clearKeywords();
        virtual bool expandPreset(size_t keywordIndex, const QStringList &presetList);
        virtual bool appendPreset(const QStringList &presetList);
        virtual bool hasKeywords(const QStringList &keywordsList);
        bool areKeywordsEmpty();
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags);
        virtual bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive);

    private:
        bool appendKeywordUnsafe(const QString &keyword);
        void takeKeywordAtUnsafe(size_t index, QString &removedKeyword, bool &wasCorrect);
        void setKeywordsUnsafe(const QStringList &keywordsList);
        size_t appendKeywordsUnsafe(const QStringList &keywordsList);
        bool canEditKeywordUnsafe(size_t index, const QString &replacement) const;
        bool editKeywordUnsafe(size_t index, const QString &replacement);
        bool replaceKeywordUnsafe(size_t index, const QString &existing, const QString &replacement);
        bool clearKeywordsUnsafe();
        bool containsKeywordUnsafe(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::Keywords);
        bool hasKeywordsSpellErrorUnsafe() const;
        bool hasKeywordsDuplicatesUnsafe() const;
        bool removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove, bool caseSensitive);
        void expandPresetUnsafe(size_t keywordsIndex, const QStringList &keywordsList);
        bool hasKeywordsUnsafe(const QStringList &keywordsList) const;
        QStringList generateStringListUnsafe();

        void lockKeywordsRead() { m_KeywordsLock.lockForRead(); }
        void unlockKeywords() { m_KeywordsLock.unlock(); }

    public:
        virtual QString retrieveKeyword(int wordIndex);
        virtual QStringList getKeywords();
        virtual void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        virtual std::vector<KeywordItem> retrieveMisspelledKeywords();
        virtual std::vector<KeywordItem> retrieveDuplicatedKeywords();
        virtual Common::KeywordReplaceResult fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement);
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval);
        virtual void connectSignals(SpellCheck::SpellCheckItem *item);
        virtual void afterReplaceCallback();

    private:
        void removeKeywordsAtIndicesUnsafe(const QVector<int> &indices);
        bool replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                     Common::SearchFlags flags);

    public:
        bool containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::ExactKeywords);
        bool containsKeywords(const QStringList &keywordsList);
        virtual bool isEmpty();
        bool hasKeywordsSpellError();
        bool hasKeywordsDuplicates();

        virtual bool hasSpellErrors();
        virtual bool hasDuplicates();

    public:
        void notifySpellCheckResults(SpellCheckFlags flags);
        void notifyAboutToBeRemoved() { emit aboutToBeRemoved(); }
        void notifyCompletionsAvailable() { emit completionsAvailable(); }

    public:
        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

    private:
        void resetSpellCheckResultsUnsafe();
        bool canBeAddedUnsafe(const QString &keyword) const;

    public:
        Q_INVOKABLE bool hasKeyword(const QString &keyword);
        Q_INVOKABLE bool canEditKeyword(int index, const QString &replacement);

    signals:
        void spellCheckResultsReady();
        void spellCheckErrorsChanged();
        void hasDuplicatesChanged();
        void completionsAvailable();
        void aboutToBeRemoved();
        void afterSpellingErrorsFixed();

    protected slots:
        void onSpellCheckRequestReady(Common::SpellCheckFlags flags, size_t index);

    private:
        void setSpellCheckResultsUnsafe(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        bool isReplacedADuplicateUnsafe(size_t index, const QString &existingPrev,
                                        const QString &replacement) const;
        void emitSpellCheckChanged(int index=-1);

    private:
        Common::Hold &m_Hold;
        QReadWriteLock m_KeywordsLock;
        std::vector<Keyword> m_KeywordsList;
        QSet<QString> m_KeywordsSet;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel *)

#endif // BASICKEYWORDSMODEL_H
