/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BASICMETADATAMODEL_H
#define BASICMETADATAMODEL_H

#include "basickeywordsmodel.h"
#include <QStringList>
#include <QHash>
#include <QReadWriteLock>
#include "flags.h"
#include "imetadataoperator.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicMetadataModel :
            public BasicKeywordsModel,
            public Common::IMetadataOperator
    {
        Q_OBJECT
        Q_PROPERTY(bool hasTitleSpellErrors READ hasTitleSpellError NOTIFY titleSpellingChanged)
        Q_PROPERTY(bool hasDescriptionSpellErrors READ hasDescriptionSpellError NOTIFY descriptionSpellingChanged)
    public:
        BasicMetadataModel(Common::Hold &hold, QObject *parent=0);

    public:
        Q_INVOKABLE bool hasAnySpellingErrors() { return hasSpellErrors(); }

    public:
        void setSpellCheckInfo(SpellCheck::SpellCheckItemInfo *info) { m_SpellCheckInfo = info; }
        SpellCheck::SpellCheckItemInfo *getSpellCheckInfo() const { return m_SpellCheckInfo; }
        QString getDescription();
        QString getTitle();

    public:
#ifdef CORE_TESTS
        void initialize(const QString &title, const QString &description, const QString &rawKeywords);
#endif

    public:
        virtual void setSpellCheckResults(const QHash<QString, Common::WordAnalysisResult> &results, SpellCheckFlags flags);
        virtual QStringList retrieveMisspelledDescriptionWords() override;
        virtual QStringList retrieveMisspelledTitleWords() override;
        virtual bool fixDescriptionSpelling(const QString &word, const QString &replacement) override;
        virtual bool fixTitleSpelling(const QString &word, const QString &replacement) override;
        virtual void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) override;
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) override;
        virtual std::vector<KeywordItem> retrieveMisspelledKeywords() override;
        virtual Common::KeywordReplaceResult fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement) override;
        virtual void afterReplaceCallback() override;
        virtual Common::BasicKeywordsModel *getBasicKeywordsModel() override;
        virtual QStringList getDescriptionWords();
        virtual QStringList getTitleWords();
        virtual bool expandPreset(size_t keywordIndex, const QStringList &presetList) override;
        virtual bool appendPreset(const QStringList &presetList) override;

    private:
        bool replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                  Common::SearchFlags flags);
        bool replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                            SearchFlags flags);

    public:
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) override;
        bool hasDescriptionSpellError();
        bool hasTitleSpellError();
        bool hasDescriptionWordSpellError(const QString &word);
        bool hasTitleWordSpellError(const QString &word);
        virtual bool hasSpellErrors() override;
        virtual bool hasDuplicates() override;

    public:
        // IMetadataOperator
        // c++ is still not capable of picking abstract implementations from Base class
        virtual bool editKeyword(size_t index, const QString &replacement) override { return BasicKeywordsModel::editKeyword(index, replacement); }
        virtual bool removeKeywordAt(size_t index, QString &removedKeyword) override { return BasicKeywordsModel::removeKeywordAt(index, removedKeyword); }
        virtual bool removeLastKeyword(QString &removedKeyword) override { return BasicKeywordsModel::removeLastKeyword(removedKeyword); }
        virtual bool appendKeyword(const QString &keyword) override { return BasicKeywordsModel::appendKeyword(keyword); }
        virtual size_t appendKeywords(const QStringList &keywordsList) override { return BasicKeywordsModel::appendKeywords(keywordsList); }
        virtual bool clearKeywords() override { return BasicKeywordsModel::clearKeywords(); }
        virtual QString getKeywordsString() override { return BasicKeywordsModel::getKeywordsString(); }
        virtual void setKeywords(const QStringList &keywords) override { return BasicKeywordsModel::setKeywords(keywords); }
        virtual bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive) override { return BasicKeywordsModel::removeKeywords(keywords, caseSensitive); }
        virtual bool hasKeywords(const QStringList &keywordsList) override { return BasicKeywordsModel::hasKeywords(keywordsList); }
        virtual void justEdited() override { /* bump */ }

    public:
        virtual bool setDescription(const QString &value) override;
        virtual bool setTitle(const QString &value) override;
        virtual bool isEmpty() override;
        bool isTitleEmpty();
        bool isDescriptionEmpty();
        void clearModel();

    public:
        virtual void notifySpellCheckResults(SpellCheckFlags flags) override;
        void notifyDescriptionSpellingChanged();
        void notifyTitleSpellingChanged();

    signals:
        void titleSpellingChanged();
        void descriptionSpellingChanged();

    private:
        void updateDescriptionSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results, bool withStemInfo);
        void updateTitleSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results, bool withStemInfo);

    private:
        QReadWriteLock m_DescriptionLock;
        QReadWriteLock m_TitleLock;
        SpellCheck::SpellCheckItemInfo *m_SpellCheckInfo;
        QString m_Description;
        QString m_Title;
    };
}

#endif // BASICMETADATAMODEL_H
