/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DBVIDEOCACHEINDEX_H
#define DBVIDEOCACHEINDEX_H

#include <QString>
#include <QHash>
#include "cachedvideo.h"
#include "../Helpers/database.h"
#include "dbcacheindex.h"

namespace QMLExtensions {
    class DbVideoCacheIndex: public DbCacheIndex<CachedVideo>
    {
    public:
        DbVideoCacheIndex(Helpers::DatabaseManager *dbManager);

    public:
        virtual bool initialize() override;

    public:
        virtual void update(const QString &originalPath, CachedVideo &cachedImage) override;

    protected:
        virtual int getMaxCacheMemorySize() const override;
    };
}

#endif // DBVIDEOCACHEINDEX_H
