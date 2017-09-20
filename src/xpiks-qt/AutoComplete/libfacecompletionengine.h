/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBFACECOMPLETIONENGINE_H
#define LIBFACECOMPLETIONENGINE_H

#include "completionenginebase.h"

class Souffleur;

namespace AutoComplete {
    class LibFaceCompletionEngine: public CompletionEngineBase
    {
    public:
        LibFaceCompletionEngine();
        virtual ~LibFaceCompletionEngine();

        // CompletionEngineBase interface
    public:
        virtual bool initialize() override;
        virtual void finalize() override;
        virtual bool generateCompletions(const CompletionQuery &query, std::vector<CompletionResult> &completions) override;

    private:
        Souffleur *m_Soufleur;
    };
}

#endif // LIBFACECOMPLETIONENGINE_H
