/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ARTWORKMETADATABACKUP_H
#define ARTWORKMETADATABACKUP_H

#include <QStringList>
#include <QString>

namespace Models { class ArtworkMetadata; }

namespace UndoRedo {
    class ArtworkMetadataBackup
    {
    public:
        ArtworkMetadataBackup(Models::ArtworkMetadata *metadata);
        ArtworkMetadataBackup(const ArtworkMetadataBackup &copy);
        virtual ~ArtworkMetadataBackup() {}

    public:
        void restore(Models::ArtworkMetadata *metadata) const;

    private:
        QString m_Description;
        QString m_Title;
        QString m_AttachedVector;
        QStringList m_KeywordsList;
        bool m_IsModified;
    };
}

#endif // ARTWORKMETADATABACKUP_H
