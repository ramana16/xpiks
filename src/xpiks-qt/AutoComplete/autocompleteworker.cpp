/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "autocompleteworker.h"
#include <QDir>
#include <QCoreApplication>
#include "../Common/defines.h"
#include <src/libfaceapi.hpp>
#include <include/types.hpp>
#include <string>
#include <QStringList>
#include "../Helpers/asynccoordinator.h"
#include "../KeywordsPresets/ipresetsmanager.h"

#define FREQUENCY_TABLE_FILENAME "en_wordlist.tsv"

namespace AutoComplete {
    AutoCompleteWorker::AutoCompleteWorker(Helpers::AsyncCoordinator *initCoordinator, KeywordsPresets::IPresetsManager *presetsManager, QObject *parent) :
        QObject(parent),
        m_InitCoordinator(initCoordinator),
        m_PresetsManager(presetsManager),
        m_Soufleur(NULL),
        m_CompletionsCount(8)
    {
        Q_ASSERT(presetsManager != nullptr);
    }

    AutoCompleteWorker::~AutoCompleteWorker() {
        if (m_Soufleur != NULL) {
            delete m_Soufleur;
        }

        LOG_INFO << "destroyed";
    }

    bool AutoCompleteWorker::initWorker() {
        LOG_INFO << "#";        

        Helpers::AsyncCoordinatorUnlocker unlocker(m_InitCoordinator);
        Q_UNUSED(unlocker);

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

    void AutoCompleteWorker::processOneItem(std::shared_ptr<CompletionQuery> &item) {
        if (!item->getNeedsUpdate()) {
            generateCompletions(item);
        } else {
            updateCompletions(item);
        }
    }

    void AutoCompleteWorker::generateCompletions(std::shared_ptr<CompletionQuery> &item) {
        LOG_INTEGR_TESTS_OR_DEBUG << item->getPrefix();
        const QString &prefix = item->getPrefix();

        vp_t completions = m_Soufleur->prompt(prefix.toStdString(), m_CompletionsCount);

        QStringList completionsList;
        QSet<QString> completionsSet;

        size_t size = completions.size(), i = 0;
        completions.reserve(size);
        completionsSet.reserve((int)size);

        for (; i < size; ++i) {
            const phrase_t &suggestion = completions[i];
            const QString phrase = QString::fromStdString(suggestion.phrase).trimmed();

            if (!completionsSet.contains(phrase)) {
                completionsList.append(phrase);
                completionsSet.insert(phrase);
            }
        }

        if (!completionsList.empty()) {
            item->setCompletions(completionsList);

            item->setNeedsUpdate();
            this->submitFirst(item);
        }
    }

    void AutoCompleteWorker::updateCompletions(std::shared_ptr<CompletionQuery> &item) {
        LOG_INTEGR_TESTS_OR_DEBUG << item->getPrefix();
        Q_ASSERT(m_PresetsManager != nullptr);

        bool anyChanges = false;
        auto &completionsList = item->getCompletions();
        int dummy;

        LOG_INTEGR_TESTS_OR_DEBUG << "Updating" << completionsList.size() << "items";

        for (auto &completion: completionsList) {
            const bool haveOnePreset = m_PresetsManager->tryFindSinglePresetByName(completion, false, dummy);
            if (haveOnePreset) {
                item->setIsPreset(completion);
                anyChanges = true;
            }
        }

        if (anyChanges) {
            LOG_INTEGR_TESTS_OR_DEBUG << "Propagating update to the model";
            item->propagateUpdates();
        }
    }
}
