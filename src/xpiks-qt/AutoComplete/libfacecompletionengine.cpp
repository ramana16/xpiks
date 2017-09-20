/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libfacecompletionengine.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <string>
#include <src/libfaceapi.hpp>
#include <include/types.hpp>
#include "../Common/defines.h"

#define FREQUENCY_TABLE_FILENAME "en_wordlist.tsv"
#define GENERATE_COMPLETIONS_COUNT 8

namespace AutoComplete {
    LibFaceCompletionEngine::LibFaceCompletionEngine():
        m_Soufleur(nullptr)
    {
    }

    LibFaceCompletionEngine::~LibFaceCompletionEngine() {
        Q_ASSERT(m_Soufleur == nullptr);
    }

    bool LibFaceCompletionEngine::initialize() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_Soufleur == nullptr);
        bool importResult = false;

        QString resourcesPath;
        QString wordlistPath;

//#if !defined(Q_OS_LINUX)
        resourcesPath = QCoreApplication::applicationDirPath();

#if defined(Q_OS_MAC)
#  if defined(INTEGRATION_TESTS)
        resourcesPath += "/../../../xpiks-qt/deps/";
#  else
        resourcesPath += "/../Resources/";
#  endif
#elif  defined(APPVEYOR)
        resourcesPath += "/../../../xpiks-qt/deps/";
#elif defined(Q_OS_WIN)
        resourcesPath += "/ac_sources/";
#elif defined(TRAVIS_CI)
        resourcesPath += "/../../xpiks-qt/deps/";
#endif

        QDir resourcesDir(resourcesPath);
        wordlistPath = resourcesDir.absoluteFilePath(FREQUENCY_TABLE_FILENAME);

        if (QFileInfo(wordlistPath).exists()) {
            try {
                m_Soufleur = new Souffleur();
                importResult = m_Soufleur->import(wordlistPath.toStdString().c_str());
                if (!importResult) {
                    LOG_WARNING << "Failed to import" << wordlistPath;
                } else {
                    LOG_INFO << "LIBFACE initialized with" << wordlistPath;
                }
            }
            catch (...) {
                LOG_WARNING << "Exception while initializing LIBFACE with" << wordlistPath;
            }
        } else {
            LOG_WARNING << "File not found:" << wordlistPath;
        }

        return importResult;
    }

    void LibFaceCompletionEngine::finalize() {
        LOG_DEBUG << "#";

        if (m_Soufleur != nullptr) {
            delete m_Soufleur;
            m_Soufleur = nullptr;
        }
    }

    bool LibFaceCompletionEngine::generateCompletions(const CompletionQuery &query, std::vector<CompletionResult> &completions) {
        Q_ASSERT(m_Soufleur != nullptr);
        const QString &prefix = query.getPrefix();

        uint_t completionsCount = GENERATE_COMPLETIONS_COUNT;
        vp_t rawCompletions = m_Soufleur->prompt(prefix.toStdString(), completionsCount);

        QSet<QString> completionsSet;
        completionsSet.reserve(completionsCount);

        const size_t size = rawCompletions.size();
        completions.reserve((int)size);
        completionsSet.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            const phrase_t &suggestion = rawCompletions[i];
            const QString phrase = QString::fromStdString(suggestion.phrase).trimmed();

            if (!completionsSet.contains(phrase)) {
                completions.push_back(CompletionResult(phrase));
                completionsSet.insert(phrase);
            }
        }

        bool anyFound = !completionsSet.isEmpty();
        return anyFound;
    }
}
