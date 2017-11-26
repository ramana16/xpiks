/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ADDARTWORKSCOMMAND_H
#define ADDARTWORKSCOMMAND_H

#include <QStringList>
#include <QHash>
#include "commandbase.h"
#include "../Common/flags.h"

namespace MetadataIO {
    class ArtworksSnapshot;
}

namespace Commands {
    class CommandManager;

    class AddArtworksCommand : public CommandBase
    {
    public:
        enum AddArtworksFlags {
            FlagAutoFindVectors = 1 << 0,
            FlagIsFullDirectory = 1 << 1,
            FlagIsSessionRestore = 1 << 2
        };

    private:
        inline bool getAutoFindVectorsFlag() const { return Common::HasFlag(m_Flags, FlagAutoFindVectors); }
        inline bool getIsFullDirectoryFlag() const { return Common::HasFlag(m_Flags, FlagIsFullDirectory); }
        inline bool getIsSessionRestore() const { return Common::HasFlag(m_Flags, FlagIsSessionRestore); }

    public:
        AddArtworksCommand(const QStringList &pathes, const QStringList &vectorPathes, Common::flag_t flags) :
            CommandBase(CommandType::AddArtworks),
            m_FilePathes(pathes),
            m_VectorsPathes(vectorPathes),
            m_Flags(flags)
        {}

        virtual ~AddArtworksCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        void afterAddedHandler(CommandManager *commandManager,
                               const MetadataIO::ArtworksSnapshot &artworksToImport,
                               QStringList filesToWatch,
                               int initialCount, int newFilesCount) const;
        void decomposeVectors(QHash<QString, QHash<QString, QString> > &vectors) const;

    public:
        QStringList m_FilePathes;
        QStringList m_VectorsPathes;
        Common::flag_t m_Flags;
    };

    class AddArtworksCommandResult : public CommandResult {
    public:
        AddArtworksCommandResult(int count):
        m_NewFilesAdded(count)
        {}
    public:
        int m_NewFilesAdded;
    };
}

#endif // ADDARTWORKSCOMMAND_H
