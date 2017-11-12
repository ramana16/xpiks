/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOVEDIRECTORYITEM_H
#define REMOVEDIRECTORYITEM_H

#include "historyitem.h"
#include "removeartworksitem.h"
#include "../Helpers/filehelpers.h"
#include "../Common/defines.h"
#include <QFileInfo>

namespace Models {
    class ArtworksRepository;
}

namespace UndoRedo {
    class RemoveDirectoryHistoryItem:
        public RemoveArtworksHistoryItem
    {
    public:
        RemoveDirectoryHistoryItem(int commandID, int startFileIndex, qint64 dirID);
        virtual ~RemoveDirectoryHistoryItem() {}

    private:
        void fillFilesAndVectors(const QString &directoryPath, bool autoFindVectors);

    public:
        virtual void undo(const Commands::ICommandManager *commandManagerInterface) override;

    public:
        virtual QString getDescription() const override {
            return QObject::tr("1 directory removed");
        }

    private:
        qint64 m_DirectoryID;
        int m_StartFileIndex;
    };
}

#endif // REMOVEDIRECTORYITEM_H
