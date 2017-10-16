/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CSVEXPORTWORKER_H
#define CSVEXPORTWORKER_H

#include <QObject>
#include <vector>
#include <memory>
#include "artworkssnapshot.h"
#include "csvexportproperties.h"

namespace MetadataIO {

    class CsvExportWorker: public QObject
    {
        Q_OBJECT
    public:
        CsvExportWorker(const std::vector<std::shared_ptr<CsvExportPlan> > &exportPlans,
                        ArtworksSnapshot &selectedArtworks,
                        QString exportDirectoryPath,
                        QObject *parent = 0);

        virtual ~CsvExportWorker();

    public:
        void dismiss() { emit stopped(); }

    public slots:
        void process() { doWork(); emit stopped(); }

    private:
        void doWork();

    signals:
        void stopped();

    private:
        const std::vector<std::shared_ptr<CsvExportPlan> > &m_ExportPlans;
        ArtworksSnapshot &m_ArtworksToExport;
        QString m_ExportDirectoryPath;
    };
}

#endif // CSVEXPORTWORKER_H
