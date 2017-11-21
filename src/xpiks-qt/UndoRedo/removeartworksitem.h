/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOVEARTWORKSITEM_H
#define REMOVEARTWORKSITEM_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QSet>
#include "historyitem.h"

namespace UndoRedo {
    class RemoveArtworksHistoryItem : public HistoryItem
    {
    public:
        RemoveArtworksHistoryItem(int commandID, const QVector<int> &removedArtworksIndices,
                                  const QStringList &removedArtworksFilepathes,
                                  const QStringList &removedAttachedVectors,
                                  const QSet<qint64> &removedSelectedDirectoryIds,
                                  bool unselectAll,
                                  bool removedAsDirectory = false):
            HistoryItem(HistoryActionType::RemovedArtworks, commandID),
            m_RemovedArtworksIndices(removedArtworksIndices),
            m_RemovedArtworksPathes(removedArtworksFilepathes),
            m_RemovedAttachedVectors(removedAttachedVectors),
            m_RemovedSelectedDirectoryIds(removedSelectedDirectoryIds),
            m_UnselectAll(unselectAll),
            m_RemovedAsDirectory(removedAsDirectory)
        {}

        virtual ~RemoveArtworksHistoryItem() { }

    public:
        virtual void undo(const Commands::ICommandManager *commandManagerInterface) override;

    public:
        virtual QString getDescription() const override {
            int count = m_RemovedArtworksIndices.length();
            return count > 1 ? QObject::tr("%1 items removed").arg(count) :
                               QObject::tr("1 item removed");
        }

    protected:
        void setRemovedArtworksIndices(const QVector<int> &value) { m_RemovedArtworksIndices = value; }
        void setRemovedArtworksPathes(const QStringList &value) { m_RemovedArtworksPathes = value; }
        void setRemovedAttachedVectors(const QStringList &value) { m_RemovedAttachedVectors = value; }
        void setRemovedSelectedDirectoryIds (const QSet<qint64> &value) { m_RemovedSelectedDirectoryIds = value; }

    private:
        QVector<int> m_RemovedArtworksIndices;
        QStringList m_RemovedArtworksPathes;
        QStringList m_RemovedAttachedVectors;
        QSet<qint64> m_RemovedSelectedDirectoryIds;
        bool m_UnselectAll;
        bool m_RemovedAsDirectory;
    };
}

#endif // REMOVEARTWORKSITEM_H
