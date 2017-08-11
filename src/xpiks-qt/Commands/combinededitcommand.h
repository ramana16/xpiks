/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMBINEDEDITCOMMAND_H
#define COMBINEDEDITCOMMAND_H

#include <QStringList>
#include <QString>
#include <vector>
#include <QVector>
#include "commandbase.h"
#include "../Models/metadataelement.h"
#include "../Common/flags.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {

    class CombinedEditCommand: public CommandBase
    {
    public:
        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            std::vector<Models::MetadataElement> &infos,
                            const QString &description, const QString &title,
                            const QStringList &keywords) :
            CommandBase(CommandType::CombinedEdit),
            m_MetadataElements(std::move(infos)),
            m_ArtworkDescription(description),
            m_ArtworkTitle(title),
            m_Keywords(keywords),
            m_EditFlags(editFlags)
        { }

        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            std::vector<Models::MetadataElement> &infos) :
            CommandBase(CommandType::CombinedEdit),
            m_MetadataElements(std::move(infos)),
            m_EditFlags(editFlags)
        { }

        virtual ~CombinedEditCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        void setKeywords(Models::ArtworkMetadata *metadata) const;
        void setDescription(Models::ArtworkMetadata *metadata) const;
        void setTitle(Models::ArtworkMetadata *metadata) const;

    private:
        std::vector<Models::MetadataElement> m_MetadataElements;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QStringList m_Keywords;
        Common::CombinedEditFlags m_EditFlags;
    };

    class CombinedEditCommandResult : public CommandResult {
    public:
        CombinedEditCommandResult(const QVector<Models::ArtworkMetadata *> &affectedItems,
                                  const QVector<Models::ArtworkMetadata *> &itemsToSave,
                                  const QVector<int> &indicesToUpdate) :
            m_AffectedItems(affectedItems),
            m_ItemsToSave(itemsToSave),
            m_IndicesToUpdate(indicesToUpdate)
        {
        }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const override;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        QVector<Models::ArtworkMetadata *> m_AffectedItems;
        QVector<Models::ArtworkMetadata *> m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // COMBINEDEDITCOMMAND_H
