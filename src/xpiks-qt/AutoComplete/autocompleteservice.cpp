/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autocompleteservice.h"
#include <QThread>
#include "autocompleteworker.h"
#include "completionquery.h"
#include "../Common/flags.h"
#include "../Common/basickeywordsmodel.h"
#include "../Helpers/asynccoordinator.h"

namespace AutoComplete {
    AutoCompleteService::AutoCompleteService(AutoCompleteModel *autoCompleteModel, QObject *parent):
        QObject(parent),
        m_AutoCompleteWorker(NULL),
        m_AutoCompleteModel(autoCompleteModel),
        m_RestartRequired(false)
    {
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

        m_AutoCompleteWorker = new AutoCompleteWorker(coordinator);

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

    void AutoCompleteService::submitItem(QString *item, int flags) {
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

    void AutoCompleteService::findKeywordCompletions(const QString &prefix, QObject *notifyObject) {
        if (m_AutoCompleteWorker == NULL) {
            LOG_WARNING << "Worker is NULL";
            return;
        }

        LOG_INFO << "Received:" << prefix;
        QString requestPrefix = prefix.toLower();
        LOG_INFO << "Requesting for" << requestPrefix;
        std::shared_ptr<CompletionQuery> query(new CompletionQuery(requestPrefix, m_AutoCompleteModel),
                                               [](CompletionQuery *cq) { cq->deleteLater(); });

        Common::BasicKeywordsModel *basicKeywordsModel = qobject_cast<Common::BasicKeywordsModel*>(notifyObject);
        Q_ASSERT(basicKeywordsModel != NULL);

        QObject::connect(query.get(), &CompletionQuery::completionsAvailable, basicKeywordsModel, &Common::BasicKeywordsModel::completionsAvailable);
        QObject::connect(query.get(), &CompletionQuery::completionsAvailable, m_AutoCompleteModel, &AutoCompleteModel::completionsArrived);

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
