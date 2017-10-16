/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "warningsservice.h"
#include <QVector>
#include "../Common/defines.h"
#include "warningscheckingworker.h"
#include "../Commands/commandmanager.h"
#include "warningsitem.h"
#include "../Common/flags.h"

namespace Warnings {
    WarningsService::WarningsService(QObject *parent):
        QObject(parent),
        m_WarningsWorker(NULL),
        m_IsStopped(false)
    {}

    void WarningsService::initWarningsSettings() {
        QTimer::singleShot(1000, this, SLOT(updateWarningsSettings()));
    }

    void WarningsService::cancelCurrentBatch() {
        LOG_DEBUG << "#";

        if (m_WarningsWorker != nullptr) {
            m_WarningsWorker->cancelPendingJobs();
        }
    }

    void WarningsService::startService(const std::shared_ptr<Common::ServiceStartParams> &params) {
        Q_UNUSED(params);
        m_WarningsWorker = new WarningsCheckingWorker(&m_WarningsSettingsModel);

        QThread *thread = new QThread();
        m_WarningsWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_WarningsWorker, &WarningsCheckingWorker::process);
        QObject::connect(m_WarningsWorker, &WarningsCheckingWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_WarningsWorker, &WarningsCheckingWorker::stopped, m_WarningsWorker, &WarningsCheckingWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(m_WarningsWorker, &WarningsCheckingWorker::destroyed,
                         this, &WarningsService::workerDestoyed);

        QObject::connect(m_WarningsWorker, &WarningsCheckingWorker::stopped,
                         this, &WarningsService::workerStopped);

        QObject::connect(m_WarningsWorker, &WarningsCheckingWorker::queueIsEmpty,
                         this, &WarningsService::queueIsEmpty);

        LOG_INFO << "Starting worker";

        thread->start();

        m_IsStopped = false;
    }

    void WarningsService::stopService() {
        if (m_WarningsWorker != NULL) {
            LOG_INFO << "Stopping worker";
            m_WarningsWorker->stopWorking();
        } else {
            LOG_WARNING << "Warnings Worker was NULL";
        }

        m_IsStopped = true;
    }

    bool WarningsService::isBusy() const {
        bool isBusy = (m_WarningsWorker != NULL) && (m_WarningsWorker->hasPendingJobs());

        return isBusy;
    }

    void WarningsService::submitItem(Models::ArtworkMetadata *item) {
        if (m_WarningsWorker == NULL) { return; }
        if (m_IsStopped) { return; }

        LOG_INFO << "Submitting one item";

        std::shared_ptr<IWarningsItem> wItem(new WarningsItem(item));
        m_WarningsWorker->submitItem(wItem);
    }

    void WarningsService::submitItem(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags) {
        if (m_WarningsWorker == NULL) { return; }
        if (m_IsStopped) { return; }

        LOG_INFO << "Submitting one item with flags" << Common::warningsFlagToString(flags);

        std::shared_ptr<IWarningsItem> wItem(new WarningsItem(item, flags));
        m_WarningsWorker->submitItem(wItem);
    }

    void WarningsService::submitItems(const MetadataIO::WeakArtworksSnapshot &items) {
        if (m_WarningsWorker == NULL) { return; }
        if (m_IsStopped) { return; }

        int length = items.length();

        std::vector<std::shared_ptr<IWarningsItem> > itemsToSubmit;
        itemsToSubmit.reserve(length);

        for (int i = 0; i < length; ++i) {
            Models::ArtworkMetadata *item = items.at(i);
            itemsToSubmit.emplace_back(new WarningsItem(item));
        }

        itemsToSubmit.emplace_back(new EmptyWarningsItem());

        LOG_INFO << "Submitting" << length << "item(s)";
        m_WarningsWorker->submitItems(itemsToSubmit);
    }

    void WarningsService::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);

        m_WarningsSettingsModel.setCommandManager(commandManager);
    }

    void WarningsService::workerDestoyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_WarningsWorker = NULL;
    }

    void WarningsService::workerStopped() {
        LOG_DEBUG << "#";
    }

    void WarningsService::updateWarningsSettings() {
        LOG_DEBUG << "#";
        m_WarningsSettingsModel.initializeConfigs();
    }
}
