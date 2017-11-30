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
#include <QVector>
#include <QHash>
#include <memory>
#include "flags.h"
#include "wordanalysisresult.h"
#include "keyword.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModelImpl;
    class Hold;

    class BasicKeywordsModel:
            public AbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(bool hasKeywordsSpellErrors READ hasKeywordsSpellError NOTIFY keywordsSpellingChanged)
        Q_PROPERTY(bool hasDuplicates READ hasDuplicates NOTIFY hasDuplicatesChanged)
        Q_PROPERTY(bool hasAnySpellingError READ hasSpellErrors NOTIFY spellingInfoUpdated)

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
        const QString &getKeywordAt(int index) const;
        std::vector<Keyword> &getRawKeywords();
#endif

#ifdef INTEGRATION_TESTS
        bool hasDuplicateAt(size_t i) const;
#endif

    public:
        virtual void removeItemsFromRanges(const QVector<QPair<int, int> > &ranges) override;
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
        virtual size_t appendKeywords(const QStringList &keywordsList);
        virtual bool editKeyword(size_t index, const QString &replacement);
        virtual bool clearKeywords();
        virtual bool expandPreset(size_t keywordIndex, const QStringList &presetList);
        virtual bool appendPreset(const QStringList &presetList);
        virtual bool hasKeywords(const QStringList &keywordsList);
        bool areKeywordsEmpty();
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags);
        virtual bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive);

    public:
        virtual QString retrieveKeyword(size_t wordIndex);
        virtual QStringList getKeywords();
        virtual void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        virtual std::vector<KeywordItem> retrieveMisspelledKeywords();
        virtual std::vector<KeywordItem> retrieveDuplicatedKeywords();
        virtual Common::KeywordReplaceResult fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement);
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval);
        virtual void connectSignals(SpellCheck::SpellCheckItem *item);
        virtual void afterReplaceCallback();

    public:
        bool containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::ExactKeywords);
        bool containsKeywords(const QStringList &keywordsList);
        virtual bool isEmpty();
        bool hasKeywordsSpellError();
        bool hasKeywordsDuplicates();

        virtual bool hasSpellErrors();
        virtual bool hasDuplicates();

    public:
        virtual void notifySpellCheckResults(SpellCheckFlags flags);
        void notifyAboutToBeRemoved() { emit aboutToBeRemoved(); }
        void notifyCompletionsAvailable() { emit completionsAvailable(); }

    protected:
        void notifyKeywordsSpellingChanged();

    public:
        void acquire();
        bool release();

    public:
        Q_INVOKABLE bool hasKeyword(const QString &keyword);
        Q_INVOKABLE bool canEditKeyword(int index, const QString &replacement);

    signals:
        void keywordsSpellingChanged();
        void spellingInfoUpdated();
        void hasDuplicatesChanged();
        void completionsAvailable();
        void aboutToBeRemoved();
        void afterSpellingErrorsFixed();

    protected slots:
        void onSpellCheckRequestReady(Common::SpellCheckFlags flags, int index);

    private:
        void updateKeywordsHighlighting(int index=-1);

    private:
        std::shared_ptr<BasicKeywordsModelImpl> m_Impl;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel *)

#endif // BASICKEYWORDSMODEL_H
