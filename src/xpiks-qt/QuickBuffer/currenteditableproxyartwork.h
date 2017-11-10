/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CURRENTEDITABLEPROXYARTWORK_H
#define CURRENTEDITABLEPROXYARTWORK_H

#include "icurrenteditable.h"

namespace Models {
    class ArtworkProxyBase;
}

namespace QuickBuffer {
    class CurrentEditableProxyArtwork : public ICurrentEditable
    {
    public:
        CurrentEditableProxyArtwork(Models::ArtworkProxyBase *artworkProxy);

        // ICurrentEditable interface
    public:
        virtual qint64 getItemID() override;

        virtual QString getTitle() override;
        virtual QString getDescription() override;
        virtual QStringList getKeywords() override;

        virtual void setTitle(const QString &title) override;
        virtual void setDescription(const QString &description) override;
        virtual void setKeywords(const QStringList &keywords) override;

        virtual bool appendPreset(KeywordsPresets::ID_t presetID) override;
        virtual bool expandPreset(int keywordIndex, KeywordsPresets::ID_t presetID) override;
        virtual bool removePreset(KeywordsPresets::ID_t presetID) override;

        virtual bool hasKeywords(const QStringList &keywordsList) override;

        virtual void spellCheck() override;
        virtual void update() override { /* BUMP */ }

    private:
        Models::ArtworkProxyBase *m_ArtworkProxy;
    };
}

#endif // CURRENTEDITABLEPROXYARTWORK_H
