/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMETADATAREADER_H
#define IMETADATAREADER_H

#include <QVector>
#include <QHash>
#include <QString>
#include "importdataresult.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class IMetadataReader {
    public:
        virtual ~IMetadataReader() {}

        virtual const QHash<QString, ImportDataResult> &getImportResult() const = 0;
        virtual const QVector<Models::ArtworkMetadata *> &getItemsToRead() const = 0;
        virtual const QVector<QPair<int, int> > &getRangesToUpdate() const = 0;
    };
}

#endif // IMETADATAREADER_H
