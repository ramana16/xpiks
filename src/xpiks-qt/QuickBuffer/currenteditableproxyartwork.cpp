/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "currenteditableproxyartwork.h"
#include "../Models/artworkproxybase.h"

namespace QuickBuffer {
    CurrentEditableProxyArtwork::CurrentEditableProxyArtwork(Models::ArtworkProxyBase *artworkProxy):
        m_ArtworkProxy(artworkProxy)
    {
        Q_ASSERT(artworkProxy != nullptr);
    }

    qint64 CurrentEditableProxyArtwork::getItemID() {
        return m_ArtworkProxy->getSpecialItemID();
    }

    QString CurrentEditableProxyArtwork::getTitle() {
        return m_ArtworkProxy->getTitle();
    }

    QString CurrentEditableProxyArtwork::getDescription() {
        return m_ArtworkProxy->getDescription();
    }

    QStringList CurrentEditableProxyArtwork::getKeywords() {
        return m_ArtworkProxy->getKeywords();
    }

    void CurrentEditableProxyArtwork::setTitle(const QString &title) {
        m_ArtworkProxy->setTitle(title);
    }

    void CurrentEditableProxyArtwork::setDescription(const QString &description) {
        m_ArtworkProxy->setDescription(description);
    }

    void CurrentEditableProxyArtwork::setKeywords(const QStringList &keywords) {
        m_ArtworkProxy->setKeywords(keywords);
    }

    bool CurrentEditableProxyArtwork::appendPreset(int presetIndex) {
        return m_ArtworkProxy->doAppendPreset(presetIndex);
    }

    bool CurrentEditableProxyArtwork::expandPreset(int keywordIndex, int presetIndex) {
        return m_ArtworkProxy->doExpandPreset(keywordIndex, presetIndex);
    }

    bool CurrentEditableProxyArtwork::removePreset(int presetIndex) {
        return m_ArtworkProxy->doRemovePreset(presetIndex);
    }

    bool CurrentEditableProxyArtwork::hasKeywords(const QStringList &keywordsList) {
        return m_ArtworkProxy->hasKeywords(keywordsList);
    }

    void CurrentEditableProxyArtwork::spellCheck() {
        m_ArtworkProxy->spellCheckEverything();
    }
}
