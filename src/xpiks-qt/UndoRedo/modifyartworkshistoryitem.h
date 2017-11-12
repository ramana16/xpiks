/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODIFYARTWORKHISTORYITEM_H
#define MODIFYARTWORKHISTORYITEM_H

#include <QObject>
#include <vector>
#include <QString>
#include <QVector>
#include "historyitem.h"
#include "artworkmetadatabackup.h"

namespace UndoRedo {
    enum ModificationType {
        PasteModificationType,
        CombinedEditModificationType
    };

    QString getModificationTypeDescription(ModificationType type);

    class ModifyArtworksHistoryItem : public HistoryItem
    {
    public:
        ModifyArtworksHistoryItem(int commandID, const std::vector<ArtworkMetadataBackup> &backups,
                                  const QVector<int> &indices,
                                  ModificationType modificationType) :
            HistoryItem(HistoryActionType::ModifyArtworks, commandID),
            m_ArtworksBackups(backups),
            m_Indices(indices),
            m_ModificationType(modificationType)
        {
            Q_ASSERT((int)backups.size() == indices.length());
            Q_ASSERT(!backups.empty());
        }

        virtual ~ModifyArtworksHistoryItem() { }

    public:
         virtual void undo(const Commands::ICommandManager *commandManagerInterface) override;

    public:
         virtual QString getDescription() const override {
             size_t count = m_ArtworksBackups.size();
             QString typeStr = getModificationTypeDescription(m_ModificationType);
             return count > 1 ? QObject::tr("(%1)  %2 items modified").arg(typeStr).arg(count) :
                                  QObject::tr("(%1)  1 item modified").arg(typeStr);
         }


    private:
        std::vector<ArtworkMetadataBackup> m_ArtworksBackups;
        QVector<int> m_Indices;
        ModificationType m_ModificationType;
    };
}

#endif // MODIFYARTWORKHISTORYITEM_H
