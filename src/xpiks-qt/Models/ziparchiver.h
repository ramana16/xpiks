/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ZIPARCHIVER_H
#define ZIPARCHIVER_H

#include <QFutureWatcher>
#include <QPair>
#include <QVector>
#include "artworksprocessor.h"

class QStringList;
class QString;

namespace Models {
    class ZipArchiver : public ArtworksProcessor
    {
        Q_OBJECT
    public:
        ZipArchiver();
        virtual ~ZipArchiver() { delete m_ArchiveCreator; }

    public:
        virtual int getItemsCount() const override;

    public slots:
        void archiveCreated(int);
        void allFinished();

    public:
        Q_INVOKABLE void archiveArtworks();
        virtual void cancelProcessing() override { /*BUMP*/ }

    private:
        void fillFilenamesHash(QHash<QString, QStringList> &hash);

    private:
        QFutureWatcher<QStringList> *m_ArchiveCreator;
    };
}

#endif // ZIPARCHIVER_H
