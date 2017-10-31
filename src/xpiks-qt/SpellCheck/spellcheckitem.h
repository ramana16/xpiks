/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKITEM_H
#define SPELLCHECKITEM_H

#include <vector>
#include <memory>
#include <QStringList>
#include <QObject>
#include <QHash>
#include <functional>
#include "../Common/flags.h"
#include "../Common/wordanalysisresult.h"

namespace Common {
    class BasicKeywordsModel;
}

namespace SpellCheck {
    class SpellCheckQueryItem
    {
    public:
        SpellCheckQueryItem(size_t index, const QString &word):
            m_Word(word),
            m_Index(index),
            m_IsCorrect(true),
            m_IsDuplicate(false)
        { }

        SpellCheckQueryItem(const SpellCheckQueryItem &copy):
            m_Word(copy.m_Word),
            m_Index(copy.m_Index),
            m_Suggestions(copy.m_Suggestions),
            m_Stem(copy.m_Stem),
            m_IsCorrect(copy.m_IsCorrect),
            m_IsDuplicate(copy.m_IsDuplicate)
        { }

        QString m_Word;
        size_t m_Index;
        QStringList m_Suggestions;
        QString m_Stem;
        volatile bool m_IsCorrect;
        volatile bool m_IsDuplicate;
    };

    class ISpellCheckItem
    {
    public:
        virtual ~ISpellCheckItem() {}
    };

    class SpellCheckItemBase:
        public QObject, public ISpellCheckItem
    {
    Q_OBJECT

    public:
        virtual ~SpellCheckItemBase();

    protected:
        SpellCheckItemBase(Common::WordAnalysisFlags wordAnalysisFlag):
            QObject(),
            m_NeedsSuggestions(false),
            m_WordAnalysisFlag(wordAnalysisFlag)
            {}

    public:
        const std::vector<std::shared_ptr<SpellCheckQueryItem> > &getQueries() const { return m_QueryItems; }
        Common::WordAnalysisFlags getWordAnalysisFlags() const { return m_WordAnalysisFlag; }

        const QHash<QString, Common::WordAnalysisResult> &getHash() const {
            return m_SpellCheckResults;
        }
        virtual void submitSpellCheckResult() = 0;

        bool needsSuggestions() const { return m_NeedsSuggestions; }
        void requestSuggestions() { m_NeedsSuggestions = true; }
        void accountResults();

    protected:
        void reserve(int n) { m_QueryItems.reserve(m_QueryItems.size() + n); }
        void appendItem(const std::shared_ptr<SpellCheckQueryItem> &item);

    private:
        std::vector<std::shared_ptr<SpellCheckQueryItem> > m_QueryItems;
        QHash<QString, Common::WordAnalysisResult> m_SpellCheckResults;
        volatile bool m_NeedsSuggestions;
        Common::WordAnalysisFlags m_WordAnalysisFlag;
    };

    class SpellCheckSeparatorItem:
        public ISpellCheckItem
    {
    public:
        virtual ~SpellCheckSeparatorItem() {}
    };

    class SpellCheckItem:
        public SpellCheckItemBase
    {
    Q_OBJECT

    public:
        SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags, Common::WordAnalysisFlags wordAnalysisFlags, int keywordIndex);
        SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, Common::SpellCheckFlags spellCheckFlags, Common::WordAnalysisFlags wordAnalysisFlags);
        SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, const QStringList &keywordsToCheck, Common::WordAnalysisFlags wordAnalysisFlags);
        virtual ~SpellCheckItem();

    private:
        void addWords(const QStringList &words, int startingIndex, const std::function<bool (const QString &word)> &pred);

    signals:
        void resultsReady(Common::SpellCheckFlags flags, int index);

    public:
        virtual void submitSpellCheckResult();

        bool getIsOnlyOneKeyword() const { return m_OnlyOneKeyword; }
        bool getWithDelay() const { return m_WithDelay; }

    public:
        void setWithDelay() { m_WithDelay = true; }

    private:
        Common::BasicKeywordsModel *m_SpellCheckable;
        Common::SpellCheckFlags m_SpellCheckFlags;
        volatile bool m_OnlyOneKeyword;
        bool m_WithDelay;
    };

    class ModifyUserDictItem:
        public ISpellCheckItem
    {
    public:
        ModifyUserDictItem(const QString &keyword);
        ModifyUserDictItem(bool clearFlag);
        ModifyUserDictItem(const QStringList &keywords);
        virtual ~ModifyUserDictItem() {}

    public:
        const QStringList &getKeywordsToAdd() const { return m_KeywordsToAdd; }
        bool getClearFlag() const { return m_ClearFlag; }

    private:
        QStringList m_KeywordsToAdd;
        bool m_ClearFlag;
    };
}

#endif // SPELLCHECKITEM_H
