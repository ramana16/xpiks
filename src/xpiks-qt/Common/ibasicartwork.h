/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IBASICARTWORK
#define IBASICARTWORK

#include <QtGlobal>
#include <QStringList>
#include <QSet>
#include <QString>

namespace Common {
    class IBasicArtwork {
    public:
        virtual ~IBasicArtwork() {}

        virtual QSet<QString> getKeywordsSet() = 0;
        virtual QStringList getKeywords() = 0;
        virtual bool isEmpty() = 0;

        virtual QString getDescription() = 0;
        virtual QString getTitle() = 0;
        virtual const QString &getFilepath() const = 0;

        virtual qint64 getItemID() const { return -1; }
    };
}

#endif // IBASICARTWORK

