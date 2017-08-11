/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CURRENTITEMARTWORK_H
#define CURRENTITEMARTWORK_H

#include "icurrenteditable.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class CommandManager;
}

namespace QuickBuffer {
    class CurrentEditableArtwork : public ICurrentEditable
    {
    public:
        CurrentEditableArtwork(Models::ArtworkMetadata *artworkMetadata, int originalIndex, Commands::CommandManager * const commandManager);
        virtual ~CurrentEditableArtwork();

    public:
        int getOriginalIndex() const { return m_OriginalIndex; }

        // ICurrentEditable interface
    public:
        virtual qint64 getItemID() override;

        virtual QString getTitle() override;
        virtual QString getDescription() override;
        virtual QStringList getKeywords() override;

        virtual void setTitle(const QString &value) override;
        virtual void setDescription(const QString &value) override;
        virtual void setKeywords(const QStringList &keywords) override;

        virtual bool appendPreset(int presetIndex) override;
        virtual bool expandPreset(int keywordIndex, int presetIndex) override;
        virtual bool removePreset(int presetIndex) override;

        virtual bool hasKeywords(const QStringList &keywordsList) override;

        virtual void spellCheck() override;
        virtual void update() override;

    private:
        Commands::CommandManager * const m_CommandManager;
        Models::ArtworkMetadata *m_ArtworkMetadata;
        int m_OriginalIndex;
    };
}

#endif // CURRENTITEMARTWORK_H
