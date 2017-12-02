/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "quickbuffer.h"
#include "../Commands/commandmanager.h"
#include "icurrenteditable.h"
#include "../Models/uimanager.h"
#include "../QuickBuffer/currenteditableartwork.h"
#include "../Models/artitemsmodel.h"

namespace QuickBuffer {
    QuickBuffer::QuickBuffer(QObject *parent) :
        QObject(parent),
        ArtworkProxyBase(),
        m_BasicModel(m_HoldPlaceholder, this)
    {
        m_BasicModel.setSpellCheckInfo(&m_SpellCheckInfo);

        QObject::connect(&m_BasicModel, &Common::BasicMetadataModel::titleSpellingChanged,
                         this, &QuickBuffer::onTitleSpellingChanged);
        QObject::connect(&m_BasicModel, &Common::BasicMetadataModel::descriptionSpellingChanged,
                         this, &QuickBuffer::onDescriptionSpellingChanged);

//        QObject::connect(&m_BasicModel, SIGNAL(completionsAvailable()),
//                         this, SIGNAL(completionsAvailable()));

        QObject::connect(&m_BasicModel, &Common::BasicMetadataModel::afterSpellingErrorsFixed,
                         this, &QuickBuffer::afterSpellingErrorsFixedHandler);
    }

    QuickBuffer::~QuickBuffer() {
        // BUMP
    }

    void QuickBuffer::afterSpellingErrorsFixedHandler() {
        // if squeezing took place after replace
        signalKeywordsCountChanged();
    }

    void QuickBuffer::onTitleSpellingChanged() {
        emit titleChanged();
    }

    void QuickBuffer::onDescriptionSpellingChanged() {
        emit descriptionChanged();
    }

    void QuickBuffer::userDictUpdateHandler(const QStringList &keywords, bool overwritten) {
        LOG_DEBUG << "#";
        doHandleUserDictChanged(keywords, overwritten);
    }

    void QuickBuffer::userDictClearedHandler() {
        LOG_DEBUG << "#";
        doHandleUserDictCleared();
    }

    void QuickBuffer::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
        emit isEmptyChanged();
    }

    void QuickBuffer::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
        emit isEmptyChanged();
    }

    bool QuickBuffer::appendKeyword(const QString &keyword) {
        bool added = doAppendKeyword(keyword);
        emit isEmptyChanged();
        return added;
    }

    void QuickBuffer::pasteKeywords(const QStringList &keywords) {
        doAppendKeywords(keywords);
        emit isEmptyChanged();
    }

    void QuickBuffer::clearKeywords() {
        doClearKeywords();
        emit isEmptyChanged();
    }

    QString QuickBuffer::getKeywordsString() {
        return doGetKeywordsString();
    }

    void QuickBuffer::initDescriptionHighlighting(QQuickTextDocument *document) {
        doCreateDescriptionHighligher(document);
    }

    void QuickBuffer::initTitleHighlighting(QQuickTextDocument *document) {
        doCreateTitleHighlighter(document);
    }

    void QuickBuffer::spellCheckDescription() {
        doSpellCheckDescription();
    }

    void QuickBuffer::spellCheckTitle() {
        doSpellCheckTitle();
    }

    bool QuickBuffer::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool QuickBuffer::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void QuickBuffer::resetModel() {
        m_BasicModel.clearModel();
        emit isEmptyChanged();
    }

    bool QuickBuffer::copyToCurrentEditable() {
        LOG_DEBUG << "#";
        bool result = false;
        auto *uiManager = m_CommandManager->getUIManager();
        auto currentEditable = uiManager->getCurrentEditable();

        auto editableArtwork = std::dynamic_pointer_cast<CurrentEditableArtwork>(currentEditable);
        if (editableArtwork) {
            auto *artItemsModel = m_CommandManager->getArtItemsModel();
            artItemsModel->fillFromQuickBuffer(editableArtwork->getOriginalIndex());
            result = true;
        } else if (currentEditable) {
            auto *model = getBasicMetadataModel();

            if (!model->isTitleEmpty()) {
                currentEditable->setTitle(getTitle());
            }

            if (!model->isDescriptionEmpty()) {
                currentEditable->setDescription(getDescription());
            }

            if (!model->areKeywordsEmpty()) {
                currentEditable->setKeywords(getKeywords());
            }

            currentEditable->spellCheck();
            currentEditable->update();

            result = true;
        } else {
            LOG_WARNING << "Nothing registered as current item";
        }

        return result;
    }

    bool QuickBuffer::getIsEmpty() {
        auto *model = getBasicMetadataModel();
        bool result = model->isTitleEmpty() && model->isDescriptionEmpty() && model->areKeywordsEmpty();
        return result;
    }

    void QuickBuffer::setFromBasicModel(Common::BasicMetadataModel *model) {
        LOG_DEBUG << "#";
        Q_ASSERT(model != nullptr);

        auto title = model->getTitle();
        auto description = model->getDescription();
        auto keywords = model->getKeywords();

        if (!title.isEmpty()) { this->setTitle(title); }
        if (!description.isEmpty()) { this->setDescription(description); }
        if (!keywords.empty()) { this->setKeywords(keywords); }

        emit isEmptyChanged();
    }

    void QuickBuffer::setFromSuggestionArtwork(const std::shared_ptr<Suggestion::SuggestionArtwork> &from) {
        LOG_DEBUG << "#";
        Q_ASSERT(from != nullptr);

        auto &title = from->getTitle();
        auto &description = from->getDescription();
        auto keywords = from->getKeywordsSet().toList();

        this->setTitle(title);
        this->setDescription(description);
        if (!keywords.empty()) { this->setKeywords(keywords); }

        emit isEmptyChanged();
    }
}
