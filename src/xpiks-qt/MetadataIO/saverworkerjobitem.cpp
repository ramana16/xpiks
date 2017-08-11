/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "saverworkerjobitem.h"
#include <QFile>
#include <QDataStream>
#include "../Helpers/constants.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/defines.h"

namespace MetadataIO {
    MetadataSavingCopy::MetadataSavingCopy(Common::BasicMetadataModel *keywordsModel) {
        readFromMetadata(keywordsModel);
    }

    MetadataSavingCopy::MetadataSavingCopy(const QHash<QString, QString> &dict):
        m_MetadataInfo(dict)
    {
    }

    void MetadataSavingCopy::saveToFile(const QString &filepath) const {
        QString path = filepath + QLatin1String(Constants::METADATA_BACKUP_EXTENSION);
        const QHash<QString, QString> &dict = m_MetadataInfo;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QDataStream out(&file);   // write the data
            out << dict;
            file.close();
        }
    }

    bool MetadataSavingCopy::readFromFile(const QString &filepath) {
        bool success = false;
        QString path = filepath + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QHash<QString, QString> dict;

            QDataStream in(&file);   // read the data
            try {
                in >> dict;
                m_MetadataInfo.swap(dict);
                success = true;
            }
            catch (...) {
                LOG_WARNING << "Exception while reading backup";
            }

            file.close();
        }

        return success;
    }

    void MetadataSavingCopy::saveToMetadata(Models::ArtworkMetadata *artworkMetadata) const {
        const QHash<QString, QString> &dict = m_MetadataInfo;

        QString keywordsString = dict.value("keywords", "");
        QStringList keywords = keywordsString.split(QChar(','), QString::SkipEmptyParts);

        if (artworkMetadata->initialize(
                dict.value("title", ""),
                dict.value("description", ""),
                keywords,
                false)) {
            artworkMetadata->markModified();
        }
    }

    void MetadataSavingCopy::readFromMetadata(Common::BasicMetadataModel *keywordsModel) {
        m_MetadataInfo["title"] = keywordsModel->getTitle();
        m_MetadataInfo["description"] = keywordsModel->getDescription();
        m_MetadataInfo["keywords"] = keywordsModel->getKeywordsString();
    }
}

