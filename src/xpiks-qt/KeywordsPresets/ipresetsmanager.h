/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IPRESETSMANAGER_H
#define IPRESETSMANAGER_H

#include <QString>
#include <QVector>
#include <QPair>

namespace KeywordsPresets {
    typedef unsigned int ID_t;

    class IPresetsManager {
    public:
        virtual ~IPresetsManager() {}

        virtual bool tryGetPreset(ID_t id, QStringList &keywords) = 0;
        virtual bool tryFindSinglePresetByName(const QString &name, bool strictMatch, ID_t &id) = 0;
        virtual void findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) = 0;
        virtual void findOrRegisterPreset(const QString &name, const QStringList &keywords, ID_t &id) = 0;
        virtual void addOrUpdatePreset(const QString &name, const QStringList &keywords, ID_t &id, bool &isAdded) = 0;
        virtual bool setPresetGroup(ID_t presetID, int groupID) = 0;
        virtual void findOrRegisterGroup(const QString &groupName, int &groupID) = 0;
        virtual void requestBackup() = 0;
        virtual void refreshPresets() = 0;
        virtual void triggerPresetsUpdated() = 0;
    };
}

#endif // IPRESETSMANAGER_H
