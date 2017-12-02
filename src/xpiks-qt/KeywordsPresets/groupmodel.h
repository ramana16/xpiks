/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETGROUP_H
#define PRESETGROUP_H

#include <QString>

#define DEFAULT_GROUP_ID -1

namespace KeywordsPresets {

    struct GroupModel {
        GroupModel(const QString &name, int groupID):
            m_Name(name),
            m_GroupID(groupID)
        { }

        QString m_Name;
        int m_GroupID;
    };
}

#endif // PRESETGROUP_H
