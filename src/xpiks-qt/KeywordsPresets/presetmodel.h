/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETMODEL_H
#define PRESETMODEL_H

#include <QString>
#include "../Common/basickeywordsmodel.h"
#include "../Common/hold.h"
#include "../Common/flags.h"
#include "groupmodel.h"

namespace KeywordsPresets {
    typedef unsigned int ID_t;

    struct PresetModel {
        enum PresetModelFlags {
            FlagIsNameDupcate = 1 << 0
        };

        PresetModel(ID_t id):
            m_KeywordsModel(m_Hold),
            m_PresetName(QObject::tr("Untitled")),
            m_ID(id),
            m_GroupID(DEFAULT_GROUP_ID),
            m_Flags(0)
        {
        }

        PresetModel(ID_t id, const QString &name):
            m_KeywordsModel(m_Hold),
            m_PresetName(name),
            m_ID(id),
            m_GroupID(DEFAULT_GROUP_ID),
            m_Flags(0)
        {}

        PresetModel(ID_t id, const QString &name, const QStringList &keywords, int groupID):
            m_KeywordsModel(m_Hold),
            m_PresetName(name),
            m_ID(id),
            m_GroupID(groupID),
            m_Flags(0)
        {
            m_KeywordsModel.setKeywords(keywords);
        }

        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

        inline bool getIsNameDuplicateFlag() const { return Common::HasFlag(m_Flags, FlagIsNameDupcate); }
        inline void setIsNameDuplicateFlag(bool value) { Common::ApplyFlag(m_Flags, value, FlagIsNameDupcate); }

        Common::BasicKeywordsModel m_KeywordsModel;
        QString m_PresetName;
        ID_t m_ID;
        int m_GroupID;
        Common::Hold m_Hold;
        Common::flag_t m_Flags;
    };
}

#endif // PRESETMODEL_H
