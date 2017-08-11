/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMETADATAOPERATOR_H
#define IMETADATAOPERATOR_H

#include <QString>
#include <QSet>
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../Common/flags.h"

namespace Common {
    class BasicKeywordsModel;

    class IMetadataOperator {
    public:
        virtual ~IMetadataOperator() {}

        virtual bool editKeyword(int index, const QString &replacement) = 0;
        virtual bool removeKeywordAt(int index, QString &removedKeyword) = 0;
        virtual bool removeLastKeyword(QString &removedKeyword) = 0;
        virtual bool appendKeyword(const QString &keyword) = 0;
        virtual int appendKeywords(const QStringList &keywordsList) = 0;
        virtual bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive) = 0;
        virtual bool clearKeywords() = 0;
        virtual QString getKeywordsString() = 0;
        virtual void setKeywords(const QStringList &keywords) = 0;
        virtual bool setDescription(const QString &value) = 0;
        virtual bool setTitle(const QString &value) = 0;
        virtual bool expandPreset(int keywordIndex, const QStringList &presetList) = 0;
        virtual bool appendPreset(const QStringList &presetList) = 0;
        virtual bool hasKeywords(const QStringList &keywordsList) = 0;
        virtual void requestBackup() = 0;

        virtual Common::BasicKeywordsModel *getBasicKeywordsModel() = 0;

        // former ispellcheckable here
        virtual Common::KeywordReplaceResult fixKeywordSpelling(int index, const QString &existing, const QString &replacement) = 0;
        virtual bool fixDescriptionSpelling(const QString &word, const QString &replacement) = 0;
        virtual bool fixTitleSpelling(const QString &word, const QString &replacement) = 0;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createDescriptionSuggestionsList() = 0;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createTitleSuggestionsList() = 0;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createKeywordsSuggestionsList() = 0;
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) = 0;
        virtual void afterReplaceCallback() = 0;
    };
}

#endif // IMETADATAOPERATOR_H
