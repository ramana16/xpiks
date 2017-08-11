/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef QUICKBUFFER_H
#define QUICKBUFFER_H

#include <QObject>
#include <QQmlEngine>
#include "../Models/artworkproxybase.h"
#include "../Common/basicmetadatamodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Suggestion/suggestionartwork.h"

namespace QuickBuffer {
    class QuickBuffer : public QObject, public Models::ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(bool isEmpty READ getIsEmpty NOTIFY isEmptyChanged)

    public:
        explicit QuickBuffer(QObject *parent = 0);
        virtual ~QuickBuffer();

    signals:
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();
        void isEmptyChanged();

    protected:
        virtual void signalDescriptionChanged() override { emit descriptionChanged(); emit isEmptyChanged(); }
        virtual void signalTitleChanged() override { emit titleChanged(); emit isEmptyChanged(); }
        virtual void signalKeywordsCountChanged() override { emit keywordsCountChanged(); emit isEmptyChanged(); }

    public slots:
        void afterSpellingErrorsFixedHandler();
        void spellCheckErrorsChangedHandler();
        void userDictUpdateHandler(const QStringList &keywords, bool overwritten);
        void userDictClearedHandler();

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE QString getKeywordsString();
        Q_INVOKABLE void initDescriptionHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void spellCheckDescription();
        Q_INVOKABLE void spellCheckTitle();
        Q_INVOKABLE bool hasTitleWordSpellError(const QString &word);
        Q_INVOKABLE bool hasDescriptionWordSpellError(const QString &word);
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE bool copyToCurrentEditable();

#ifdef INTEGRATION_TESTS
    public:
        bool hasSpellErrors() { return m_BasicModel.hasSpellErrors(); }
#endif

    public:
        bool getIsEmpty();
        void setFromCurrentEditable();
        void setFromBasicModel(Common::BasicMetadataModel *model);
        void setFromSuggestionArtwork(const std::shared_ptr<Suggestion::SuggestionArtwork> &from);

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() override { return &m_BasicModel; }
        virtual Common::IMetadataOperator *getMetadataOperator() override { return &m_BasicModel; }

    private:
        Common::Hold m_HoldPlaceholder;
        Common::BasicMetadataModel m_BasicModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
    };
}

#endif // QUICKBUFFER_H
