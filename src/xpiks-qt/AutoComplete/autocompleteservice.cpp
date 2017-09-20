/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "autocompleteservice.h"
#include <QThread>
#include "autocompleteworker.h"
#include "completionquery.h"
#include "../Common/flags.h"
#include "../Common/basickeywordsmodel.h"
#include "../Helpers/asynccoordinator.h"
#include "../Models/settingsmodel.h"

namespace AutoComplete {
    AutoCompleteService::AutoCompleteService(KeywordsAutoCompleteModel *autoCompleteModel,
                                             KeywordsPresets::PresetKeywordsModel *presetsManager,
                                             Models::SettingsModel *settingsModel,
                                             QObject *parent):
        QObject(parent),
        m_AutoCompleteWorker(NULL),
        m_AutoCompleteModel(autoCompleteModel),
        m_PresetsManager(presetsManager),
        m_SettingsModel(settingsModel),
        m_RestartRequired(false)
    {
        Q_ASSERT(autoCompleteModel != nullptr);
        Q_ASSERT(presetsManager != nullptr);
        Q_ASSERT(settingsModel != nullptr);
    }

    AutoCompleteService::~AutoCompleteService() {
    }

    void AutoCompleteService::startService(const std::shared_ptr<Common::ServiceStartParams> &params) {
        if (m_AutoCompleteWorker != NULL) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        auto coordinatorParams = std::dynamic_pointer_cast<Helpers::AsyncCoordinatorStartParams>(params);
        Helpers::AsyncCoordinator *coordinator = nullptr;
        if (coordinatorParams) { coordinator = coordinatorParams->m_Coordinator; }

        Helpers::AsyncCoordinatorLocker locker(coordinator);
        Q_UNUSED(locker);

        m_AutoCompleteWorker = new AutoCompleteWorker(coordinator, m_AutoCompleteModel, m_PresetsManager);

        QThread *thread = new QThread();
        m_AutoCompleteWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_AutoCompleteWorker, &AutoCompleteWorker::process);
        QObject::connect(m_AutoCompleteWorker, &AutoCompleteWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_AutoCompleteWorker, &AutoCompleteWorker::stopped, m_AutoCompleteWorker, &AutoCompleteWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &AutoCompleteService::cancelAutoCompletion,
                         m_AutoCompleteWorker, &AutoCompleteWorker::cancel);

        QObject::connect(m_AutoCompleteWorker, &AutoCompleteWorker::stopped,
                         this, &AutoCompleteService::workerFinished);

        QObject::connect(m_AutoCompleteWorker, &AutoCompleteWorker::destroyed,
                         this, &AutoCompleteService::workerDestroyed);

        LOG_DEBUG << "starting thread...";
        thread->start();

        emit serviceAvailable(m_RestartRequired);
    }

    void AutoCompleteService::stopService() {
        LOG_DEBUG << "#";
        if (m_AutoCompleteWorker != NULL) {
            m_AutoCompleteWorker->stopWorking();
        } else {
            LOG_WARNING << "AutoComplete Worker was NULL";
        }
    }

    bool AutoCompleteService::isBusy() const {
        bool isBusy = (m_AutoCompleteWorker != NULL) && m_AutoCompleteWorker->hasPendingJobs();
        return isBusy;
    }

    void AutoCompleteService::submitItem(QString *item) {
        Q_UNUSED(item);
    }

    void AutoCompleteService::submitItem(QString *item, Common::flag_t flags) {
        Q_UNUSED(flags);
        this->submitItem(item);
    }

    void AutoCompleteService::submitItems(const QVector<QString *> &items) {
        Q_UNUSED(items);
#if QT_NO_DEBUG
        LOG_WARNING << "Cannot use this API for autocomplete";
#else
        Q_ASSERT(false);
#endif
    }

    void AutoCompleteService::restartWorker() {
        m_RestartRequired = true;
        stopService();
    }

    void AutoCompleteService::generateCompletions(const QString &prefix, Common::BasicKeywordsModel *basicModel) {
        if (m_AutoCompleteWorker == NULL) {
            LOG_WARNING << "Worker is NULL";
            return;
        }

        const bool completeKeywords = m_SettingsModel->getUseKeywordsAutoComplete();
        const bool completePresets = m_SettingsModel->getUsePresetsAutoComplete();

        LOG_INTEGR_TESTS_OR_DEBUG << "Complete keywords:" << completeKeywords << "presets:" << completePresets;

        if (!completeKeywords && !completePresets) {
            LOG_INTEGR_TESTS_OR_DEBUG << "Completions are disabled";
            return;
        }

        LOG_INFO << "Received:" << prefix;
        QString requestPrefix = prefix.toLower();
        LOG_INFO << "Requesting for" << requestPrefix;

        std::shared_ptr<CompletionQuery> query(new CompletionQuery(requestPrefix, basicModel));
        query->setCompleteKeywords(completeKeywords);
        query->setCompletePresets(completePresets);
        m_AutoCompleteWorker->submitItem(query);
    }

    void AutoCompleteService::workerFinished() {
        LOG_INFO << "#";
    }

    void AutoCompleteService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_AutoCompleteWorker = NULL;

        if (m_RestartRequired) {
            LOG_INFO << "Restarting worker...";
            startService(std::shared_ptr<Common::ServiceStartParams>());
            m_RestartRequired = false;
        }
    }
}
