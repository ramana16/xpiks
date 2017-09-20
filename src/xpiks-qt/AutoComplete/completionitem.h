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
#include "../Common/flags.h"

namespace AutoComplete {
    class CompletionItem {
    private:
        enum CompletionsFlags {
            FlagIsPreset = 1 << 0,
            FlagIsKeyword = 1 << 1,
            FlagShouldExpandPreset = 1 << 2
        };

        inline bool getIsPresetFlag() const { return Common::HasFlag(m_CompletionFlags, FlagIsPreset); }
        inline bool getIsKeywordFlag() const { return Common::HasFlag(m_CompletionFlags, FlagIsKeyword); }
        inline bool getShouldExpandPresetFlag() const { return Common::HasFlag(m_CompletionFlags, FlagShouldExpandPreset); }

        inline void setIsPresetFlag(bool value) { Common::ApplyFlag(m_CompletionFlags, value, FlagIsPreset); }
        inline void setIsKeywordFlag(bool value) { Common::ApplyFlag(m_CompletionFlags, value, FlagIsKeyword); }
        inline void setShouldExpandPresetFlag(bool value) { Common::ApplyFlag(m_CompletionFlags, value, FlagShouldExpandPreset); }

    public:
        CompletionItem(const QString &completion, int id):
            m_Completion(completion),
            m_CompletionFlags(0),
            m_ID(id),
            m_PresetIndex(-1)
        {}

    public:
        const QString &getCompletion() const { return m_Completion; }
        int getID() const { return m_ID; }
        int getPresetIndex() const { return m_PresetIndex; }
        bool isPreset() const { return getIsPresetFlag() && !getIsKeywordFlag(); }
        bool isKeyword() const { return getIsKeywordFlag(); }
        bool canBePreset() const { return getIsPresetFlag(); }
        bool shouldExpandPreset() const { return isPreset() || getShouldExpandPresetFlag(); }

    public:
        void setIsKeyword() { setIsKeywordFlag(true); }
        void setCanBePreset() { setIsPresetFlag(true); }
        void setIsPreset() { setIsPresetFlag(true); }
        void setShouldExpandPreset() { setShouldExpandPresetFlag(true); }
        void setPresetIndex(int index) { m_PresetIndex = index; }

    private:
        QString m_Completion;
        Common::flag_t m_CompletionFlags;
        int m_ID;
        int m_PresetIndex;
    };
}

#endif // COMPLETIONITEM_H
