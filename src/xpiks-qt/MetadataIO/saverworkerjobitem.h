/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SAVERWORKERJOBITEM
#define SAVERWORKERJOBITEM

#include <QHash>
#include <QString>
#include "../Models/artworkmetadata.h"

namespace Common {
    class BasicMetadataModel;
}

namespace MetadataIO {
    class MetadataSavingCopy {
    public:
        MetadataSavingCopy() {}
        MetadataSavingCopy(Common::BasicMetadataModel *keywordsModel);
        MetadataSavingCopy(const QHash<QString, QString> &dict);

    public:
        const QHash<QString, QString> &getInfo() const { return m_MetadataInfo; }

        void saveToFile(const QString &filepath) const;
        bool readFromFile(const QString &filepath);
        void saveToMetadata(Models::ArtworkMetadata *artworkMetadata) const;

    private:
        void readFromMetadata(Common::BasicMetadataModel *keywordsModel);

    private:
        QHash<QString, QString> m_MetadataInfo;
    };

    class SaverWorkerJobItem: public Models::ArtworkMetadataLocker
    {
    public:
        SaverWorkerJobItem(Models::ArtworkMetadata *metadata):
            Models::ArtworkMetadataLocker(metadata)
        {
        }
    };
}

#endif // SAVERWORKERJOBITEM

