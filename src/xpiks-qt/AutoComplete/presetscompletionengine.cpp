/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "presetscompletionengine.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../Helpers/stringhelper.h"

#define MAX_PRESETS_IN_AC_COUNT 10
#define PRESET_SIMILARITY_THRESHOLD 80

namespace AutoComplete {
    PresetsCompletionEngine::PresetsCompletionEngine(KeywordsPresets::PresetKeywordsModel *presetsModel):
        m_PresetsModel(presetsModel)
    {
        Q_ASSERT(presetsModel != nullptr);
    }

    bool PresetsCompletionEngine::initialize() {
        return true;
    }

    bool PresetsCompletionEngine::generateCompletions(const CompletionQuery &query, std::vector<CompletionResult> &completions) {
        QString searchTerm = query.getPrefix();

        if (searchTerm.startsWith(PRESETS_COMPLETE_PREFIX)) {
            searchTerm.remove(0, sizeof(PRESETS_COMPLETE_PREFIX));
        }

        completions.reserve(MAX_PRESETS_IN_AC_COUNT);

        const size_t initialSize = completions.size();

        m_PresetsModel->foreachPreset([&completions, &searchTerm](size_t, KeywordsPresets::PresetModel *preset) {
            const QString &presetName = preset->m_PresetName;

            bool canAdd = false;

            if (searchTerm.isEmpty() ||
                    presetName.contains(searchTerm, Qt::CaseInsensitive)) {
                canAdd = true;
            } else {
                int percentsSimilar = Helpers::levensteinPercentage(presetName, searchTerm);
                int percentageThreshold = PRESET_SIMILARITY_THRESHOLD;

                if (searchTerm.length() < presetName.length()) {
                    percentageThreshold = (PRESET_SIMILARITY_THRESHOLD * searchTerm.length()) / presetName.length();
                }

                canAdd = percentsSimilar >= percentageThreshold;
            }

            bool shouldContinue = true;

            if (canAdd) {
                completions.push_back(CompletionResult(presetName, preset->m_ID));

                if (completions.size() >= MAX_PRESETS_IN_AC_COUNT) {
                    shouldContinue = false;
                }
            }

            return shouldContinue;
        });

        return completions.size() > initialSize;
    }
}
