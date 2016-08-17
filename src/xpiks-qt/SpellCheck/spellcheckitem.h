/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SPELLCHECKITEM_H
#define SPELLCHECKITEM_H

#include <vector>
#include <memory>
#include <QStringList>
#include <QObject>
#include <QHash>

namespace Common {
    class BasicKeywordsModel;
}

namespace SpellCheck {
    class ISpellCheckable;

    class SpellCheckQueryItem {
    public:
        SpellCheckQueryItem(int index, const QString &word) :
            m_Word(word),
            m_Index(index),
            m_IsCorrect(true)
        { }

        SpellCheckQueryItem(const SpellCheckQueryItem &copy) :
            m_Word(copy.m_Word),
            m_Index(copy.m_Index),
            m_IsCorrect(copy.m_IsCorrect),
            m_Suggestions(copy.m_Suggestions)
        { }

        QString m_Word;
        int m_Index;
        volatile bool m_IsCorrect;
        QStringList m_Suggestions;
    };

    class ISpellCheckItem {
    public:
        virtual ~ISpellCheckItem() {}
    };

    class SpellCheckItemBase : public QObject, public ISpellCheckItem {
        Q_OBJECT
    public:
        virtual ~SpellCheckItemBase();

    protected:
        SpellCheckItemBase() :
            QObject(),
            m_NeedsSuggestions(false) { }

    public:
        const std::vector<std::shared_ptr<SpellCheckQueryItem> > &getQueries() const { return m_QueryItems; }
        const QHash<QString, bool> &getHash() const { return m_SpellCheckResults; }
        virtual void submitSpellCheckResult() = 0;
        bool needsSuggestions() const { return m_NeedsSuggestions; }
        void requestSuggestions() { m_NeedsSuggestions = true; }
        void accountResultAt(int index);
        bool getIsCorrect(const QString &word) const;

    protected:
        void reserve(int n) { m_QueryItems.reserve(m_QueryItems.size() + n); }
        void appendItem(const std::shared_ptr<SpellCheckQueryItem> &item);

    private:
        std::vector<std::shared_ptr<SpellCheckQueryItem> > m_QueryItems;
        QHash<QString, bool> m_SpellCheckResults;
        volatile bool m_NeedsSuggestions;
    };

    class SpellCheckSeparatorItem : public ISpellCheckItem {
    public:
        virtual ~SpellCheckSeparatorItem() {}
    };

    class SpellCheckItem : public SpellCheckItemBase {
        Q_OBJECT
    public:
        SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, int spellCheckFlags, int keywordIndex);
        SpellCheckItem(Common::BasicKeywordsModel *spellCheckable, int spellCheckFlags);
        virtual ~SpellCheckItem();

    private:
        void addWords(const QStringList &words, int startingIndex);

    signals:
        void resultsReady(int flags, int index);

    public:
        virtual void submitSpellCheckResult();
        bool getIsOnlyOneKeyword() const { return m_OnlyOneKeyword; }

    private:
        Common::BasicKeywordsModel *m_SpellCheckable;
        int m_SpellCheckFlags;
        volatile bool m_OnlyOneKeyword;
    };
}

#endif // SPELLCHECKITEM_H
