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
    class IPresetsManager {
    public:
        virtual ~IPresetsManager() {}

        virtual bool tryGetPreset(int presetIndex, QStringList &keywords) = 0;
        virtual bool tryFindSinglePresetByName(const QString &name, bool strictMatch, int &index) = 0;
        virtual void findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) = 0;
        virtual void findOrRegisterPreset(const QString &name, const QStringList &keywords, int &index) = 0;
        virtual void addOrUpdatePreset(const QString &name, const QStringList &keywords, int &index, bool &isAdded) = 0;
        virtual void requestBackup() = 0;
        virtual void refreshPresets() = 0;
        virtual void triggerPresetsUpdated() = 0;
    };
}

#endif // IPRESETSMANAGER_H
