/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKPROXYBASE_H
#define ARTWORKPROXYBASE_H

#include <QQuickTextDocument>
#include "../Common/baseentity.h"
#include "../Common/ibasicartwork.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/imetadataoperator.h"
#include "../QuickBuffer/currenteditableproxyartwork.h"

namespace Models {
    class ArtworkProxyBase:
            public virtual Common::BaseEntity
    {
    public:
        virtual ~ArtworkProxyBase() {}

    public:
        QString getDescription();
        QString getTitle();
        QStringList getKeywords();
        int getKeywordsCount();
        virtual void setDescription(const QString &description);
        virtual void setTitle(const QString &title);
        virtual void setKeywords(const QStringList &keywords);

    protected:
        virtual bool doSetDescription(const QString &description);
        virtual bool doSetTitle(const QString &title);
        virtual void doSetKeywords(const QStringList &keywords);

        // TODO: virtual inheritance from qobject is not supported
    //signals:
        virtual void signalDescriptionChanged() = 0;
        virtual void signalTitleChanged() = 0;
        virtual void signalKeywordsCountChanged() = 0;

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() = 0;
        virtual Common::IMetadataOperator *getMetadataOperator() = 0;

    protected:
        virtual Common::ID_t getSpecialItemID();

    protected:
        bool doEditKeyword(int index, const QString &replacement);
        bool doRemoveKeywordAt(int keywordIndex, QString &keyword);
        bool doRemoveLastKeyword(QString &keyword);
        bool doAppendKeyword(const QString &keyword);
        int doAppendKeywords(const QStringList &keywords);
        bool doRemoveKeywords(const QSet<QString> &keywords, bool caseSensitive);
        bool doClearKeywords();
        QString doGetKeywordsString();
        void doSuggestCorrections();
        void doInitDescriptionHighlighting(QQuickTextDocument *document);
        void doInitTitleHighlighting(QQuickTextDocument *document);
        void doSpellCheckDescription();
        void doSpellCheckTitle();
        void doPlainTextEdit(const QString &rawKeywords, bool spaceIsSeparator);
        bool getHasTitleWordSpellError(const QString &word);
        bool getHasDescriptionWordSpellError(const QString &word);
        bool doExpandPreset(int keywordIndex, int presetIndex);
        bool doAppendPreset(int presetIndex);
        bool doExpandLastKeywordAsPreset();
        bool doAddPreset(int presetIndex);
        bool doRemovePreset(int presetIndex);
        void doInitSuggestion();
        void doRegisterAsCurrentItem();
        void doHandleUserDictChanged(const QStringList &keywords, bool overwritten);
        void doHandleUserDictCleared();
        void doCopyToQuickBuffer();
        bool hasKeywords(const QStringList &keywordsList);
        void doRequestBackup();

    private:
        void spellCheckEverything();
        void spellCheckKeywords();

        friend class QuickBuffer::CurrentEditableProxyArtwork;
    };
}

#endif // ARTWORKPROXYBASE_H
