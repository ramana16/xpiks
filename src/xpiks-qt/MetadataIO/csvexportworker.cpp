/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "csvexportworker.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include "csvexportplansmodel.h"
#include "../Models/artworkmetadata.h"

#define DOUBLE_QUOTE "\""

namespace MetadataIO {
    void writeColumnNames(QFile &csvFile, const std::shared_ptr<CsvExportPlan> &plan) {
        auto &properties = plan->m_PropertiesToExport;

        const size_t size = properties.size();
        Q_ASSERT(size != 0);

        QString columnName = properties[0].m_ColumnName;
        csvFile.write(DOUBLE_QUOTE);
        csvFile.write(columnName.toUtf8());
        csvFile.write(DOUBLE_QUOTE);

        for (size_t i = 1; i < size; ++i) {
            columnName = properties[i].m_ColumnName;

            csvFile.write(", " DOUBLE_QUOTE);
            csvFile.write(columnName.toUtf8());
            csvFile.write(DOUBLE_QUOTE);
        }

        // rfc 4180 - eol should be dos-style
        csvFile.write("\r\n");
    }

    QString retrieveArtworkProperty(Models::ArtworkMetadata *artwork, CsvExportPropertyType property) {
        switch (property) {
        case Empty: return QString();
        case Filename: return artwork->getBaseFilename();
        case Title: return artwork->getTitle();
        case Description: return artwork->getDescription();
        case Keywords: return artwork->getKeywordsString();
        case Category1: return QString();
        case Category2: return QString();
        default: return QString();
        }
    }

    void writeRows(QFile &csvFile, const std::shared_ptr<CsvExportPlan> &plan, const ArtworksSnapshot::Container &artworks) {
        auto &properties = plan->m_PropertiesToExport;
        const size_t propertiesSize = properties.size();
        Q_ASSERT(propertiesSize != 0);

        for (auto &locker: artworks) {
            Models::ArtworkMetadata *artwork = locker->getArtworkMetadata();

            QString value = retrieveArtworkProperty(artwork, properties[0].m_PropertyType);
            csvFile.write(DOUBLE_QUOTE);
            csvFile.write(value.toUtf8());
            csvFile.write(DOUBLE_QUOTE);

            for (size_t i = 1; i < propertiesSize; ++i) {
                value = retrieveArtworkProperty(artwork, properties[i].m_PropertyType);

                csvFile.write("," DOUBLE_QUOTE);
                csvFile.write(value.toUtf8());
                csvFile.write(DOUBLE_QUOTE);
            }

            // rfc 4180 - eol should be dos-style
            csvFile.write("\r\n");
        }
    }

    void runExportPlan(const std::shared_ptr<CsvExportPlan> &plan, const QString &filepath, const ArtworksSnapshot::Container &artworks) {
        LOG_DEBUG << "Plan" << plan->m_Name << ": exporting" << artworks.size() << "artwork(s) to" << filepath;
        Q_ASSERT(plan->m_IsSelected);

        QFile csvFile(filepath);
        if (csvFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            writeColumnNames(csvFile, plan);
            writeRows(csvFile, plan, artworks);

            csvFile.flush();
            csvFile.close();
        } else {
            LOG_WARNING << "Failed to open" << filepath;
        }
    }

    QString filenameForPlan(const std::shared_ptr<CsvExportPlan> &plan) {
        Q_ASSERT(!plan->m_Name.trimmed().isEmpty());
#ifndef INTEGRATION_TESTS
        QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmm");
#else
        QString time = "now";
#endif
        QString result = QString("%1-%2-xpks.csv").arg(plan->m_Name).arg(time);
        return result;
    }

    CsvExportWorker::CsvExportWorker(const std::vector<std::shared_ptr<CsvExportPlan> > &exportPlans,
                                     ArtworksSnapshot &selectedArtworks,
                                     QString exportDirectoryPath,
                                     QObject *parent):
        QObject(parent),
        m_ExportPlans(exportPlans),
        m_ArtworksToExport(selectedArtworks),
        m_ExportDirectoryPath(exportDirectoryPath)
    {
    }

    CsvExportWorker::~CsvExportWorker() {
        LOG_DEBUG << "#";
    }

    void CsvExportWorker::doWork() {
        LOG_DEBUG << "#";

        QDir directory(m_ExportDirectoryPath);
        Q_ASSERT(directory.exists());
        if (!directory.exists()) {
            LOG_WARNING << "Directory" << m_ExportDirectoryPath << "does not exist. Aborting export...";
            return;
        }

        for (auto &plan: m_ExportPlans) {
            if (plan->m_IsSelected) {
                Q_ASSERT(!plan->m_PropertiesToExport.empty());
                if (plan->m_PropertiesToExport.empty()) {
                    LOG_WARNING << "Plan" << plan->m_Name << "has no properties to export. Skipping...";
                    continue;
                }

                const QString filename = filenameForPlan(plan);
                QString fullFilePath = directory.filePath(filename);

                try {
                    runExportPlan(plan, fullFilePath, m_ArtworksToExport.getRawData());
                } catch(...) {
                    LOG_WARNING << "Exception while exporting" << plan->m_Name << "to CSV";
                }
            }
        }

        if ((m_ExportPlans.size() * m_ArtworksToExport.size()) < 1000) {
            // simulate working for the spinner to show up
            QThread::usleep(1000);
        }
    }
}
