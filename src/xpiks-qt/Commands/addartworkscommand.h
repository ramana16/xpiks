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

namespace Commands {
    class AddArtworksCommand : public CommandBase
    {
    public:
        AddArtworksCommand(const QStringList &pathes, const QStringList &vectorPathes, bool autoDetectVectors) :
            CommandBase(CommandType::AddArtworks),
            m_FilePathes(pathes),
            m_VectorsPathes(vectorPathes),
            m_AutoDetectVectors(autoDetectVectors)
        {}

        virtual ~AddArtworksCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        void decomposeVectors(QHash<QString, QHash<QString, QString> > &vectors) const;

    public:
        QStringList m_FilePathes;
        QStringList m_VectorsPathes;
        bool m_AutoDetectVectors;
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
