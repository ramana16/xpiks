/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPLETIONENGINEBASE_H
#define COMPLETIONENGINEBASE_H

#include <vector>
#include "completionquery.h"

namespace AutoComplete {
    class CompletionEngineBase {
    public:
        virtual ~CompletionEngineBase() {}

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
        virtual bool generateCompletions(const CompletionQuery &query, std::vector<CompletionResult> &completions) = 0;
    };
}

#endif // COMPLETIONENGINEBASE_H
