/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETSCOMPLETIONENGINE_H
#define PRESETSCOMPLETIONENGINE_H

#include "completionenginebase.h"

#define PRESETS_COMPLETE_PREFIX "pr:"

namespace KeywordsPresets {
    class PresetKeywordsModel;
}

namespace AutoComplete {
    class PresetsCompletionEngine: public CompletionEngineBase
    {
    public:
        PresetsCompletionEngine(KeywordsPresets::PresetKeywordsModel *presetsModel);

        // CompletionEngineBase interface
    public:
        virtual bool initialize() override;
        virtual void finalize() override {}
        virtual bool generateCompletions(const CompletionQuery &query, std::vector<CompletionResult> &completions) override;

    private:
        KeywordsPresets::PresetKeywordsModel *m_PresetsModel;
    };
}

#endif // PRESETSCOMPLETIONENGINE_H
