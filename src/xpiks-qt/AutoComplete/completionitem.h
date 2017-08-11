/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPLETIONITEM_H
#define COMPLETIONITEM_H

#include <QString>

namespace AutoComplete {
    class CompletionItem {
    public:
        CompletionItem(const QString &completion):
            m_Completion(completion),
            m_IsPreset(false)
        {}

    public:
        const QString &getCompletion() const { return m_Completion; }
        bool isPreset() const { return m_IsPreset; }
        void setIsPreset() { m_IsPreset = true; }

    private:
        QString m_Completion;
        volatile bool m_IsPreset;
    };
}

#endif // COMPLETIONITEM_H
