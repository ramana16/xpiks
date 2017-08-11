/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ICURRENTEDITABLE_H
#define ICURRENTEDITABLE_H

#include <QString>

namespace QuickBuffer {
    class ICurrentEditable {
    public:
        virtual ~ICurrentEditable() {}

        virtual qint64 getItemID() = 0;

        virtual QString getTitle() = 0;
        virtual QString getDescription() = 0;
        virtual QStringList getKeywords() = 0;

        virtual void setTitle(const QString &title) = 0;
        virtual void setDescription(const QString &description) = 0;
        virtual void setKeywords(const QStringList &keywords) = 0;

        virtual bool appendPreset(int presetIndex) = 0;
        virtual bool expandPreset(int keywordIndex, int presetIndex) = 0;
        virtual bool removePreset(int presetIndex) = 0;

        virtual bool hasKeywords(const QStringList &keywordsList) = 0;

        virtual void spellCheck() = 0;
        virtual void update() = 0;
    };
}

#endif // ICURRENTEDITABLE_H
