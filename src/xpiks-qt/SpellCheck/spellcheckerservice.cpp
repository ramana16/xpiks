/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "spellcheckerservice.h"
#include "../Models/artworkmetadata.h"
#include "spellcheckworker.h"
#include "spellcheckitem.h"
#include "../Common/defines.h"
#include "../Common/flags.h"

namespace SpellCheck {
    SpellCheckerService::SpellCheckerService(Models::SettingsModel *settingsModel):
        m_SpellCheckWorker(NULL),
        m_SettingsModel(settingsModel),
        m_RestartRequired(false)
    {}

    SpellCheckerService::~SpellCheckerService() {
        if (m_SpellCheckWorker != nullptr) {}
    }

    void SpellCheckerService::startService(const std::shared_ptr<Common::ServiceStartParams> &params) {
        if (m_SpellCheckWorker != NULL) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        auto coordinatorParams = std::dynamic_pointer_cast<Helpers::AsyncCoordinatorStartParams>(params);
        Helpers::AsyncCoordinator *coordinator = nullptr;
        if (coordinatorParams) { coordinator = coordinatorParams->m_Coordinator; }

        m_SpellCheckWorker = new SpellCheckWorker(coordinator, m_SettingsModel);
        Helpers::AsyncCoordinatorLocker locker(coordinator);
        Q_UNUSED(locker);

        QThread *thread = new QThread();
        m_SpellCheckWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_SpellCheckWorker, &SpellCheckWorker::process);
        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::stopped, m_SpellCheckWorker, &SpellCheckWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &SpellCheckerService::cancelSpellChecking,
                         m_SpellCheckWorker, &SpellCheckWorker::cancel);

        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::queueIsEmpty,
                         this, &SpellCheckerService::spellCheckQueueIsEmpty);

        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::stopped,
                         this, &SpellCheckerService::workerFinished);
        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::destroyed,
                         this, &SpellCheckerService::workerDestroyed);

        // user dict
        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::wordsNumberChanged,
                         this, &SpellCheckerService::wordsNumberChangedHandler);
        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::userDictUpdate,
                         this, &SpellCheckerService::userDictUpdate);
        QObject::connect(m_SpellCheckWorker, &SpellCheckWorker::userDictCleared,
                         this, &SpellCheckerService::userDictCleared);

        LOG_DEBUG << "starting thread...";
        thread->start();

        emit serviceAvailable(m_RestartRequired);
    }

    void SpellCheckerService::stopService() {
        LOG_DEBUG << "#";
        if (m_SpellCheckWorker != NULL) {
            m_SpellCheckWorker->stopWorking();
        } else {
            LOG_WARNING << "SpellCheckWorker is NULL";
        }
    }

    bool SpellCheckerService::isBusy() const {
        bool isBusy = (m_SpellCheckWorker != NULL) && (m_SpellCheckWorker->hasPendingJobs());

        return isBusy;
    }

    void SpellCheckerService::submitItem(Common::BasicKeywordsModel *itemToCheck) {
        this->submitItem(itemToCheck, Common::SpellCheckFlags::All);
    }

    void SpellCheckerService::submitItem(Common::BasicKeywordsModel *itemToCheck, Common::SpellCheckFlags flags) {
        if (m_SpellCheckWorker == NULL) {
            return;
        }

        Q_ASSERT(itemToCheck != nullptr);
        LOG_INFO << "flags:" << (int)flags;

        std::shared_ptr<SpellCheckItem> item(new SpellCheckItem(itemToCheck, flags),
            [](SpellCheckItem *spi) {
            LOG_INTEGRATION_TESTS << "Delete later for single spellcheck item";
            spi->disconnect();
            spi->deleteLater();
        });
        itemToCheck->connectSignals(item.get());
        m_SpellCheckWorker->submitItem(item);
    }

    void SpellCheckerService::submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck) {
        if (m_SpellCheckWorker == NULL) {
            return;
        }

        std::vector<std::shared_ptr<ISpellCheckItem> > items;
        int length = itemsToCheck.length();

        items.reserve(length);
        auto deleter = [](SpellCheckItem *spi) {
            LOG_INTEGRATION_TESTS << "Delete later for multiple spellcheck item";
            spi->disconnect();
            spi->deleteLater();
        };

        for (int i = 0; i < length; ++i) {
            auto *itemToCheck = itemsToCheck.at(i);
            Q_ASSERT(itemToCheck != nullptr);
            std::shared_ptr<SpellCheckItem> item(new SpellCheckItem(itemToCheck, Common::SpellCheckFlags::All),
                deleter);
            itemToCheck->connectSignals(item.get());
            items.emplace_back(std::dynamic_pointer_cast<ISpellCheckItem>(item));
        }

        LOG_INFO << length << "item(s)";

        m_SpellCheckWorker->submitItems(items);
        m_SpellCheckWorker->submitItem(std::shared_ptr<ISpellCheckItem>(new SpellCheckSeparatorItem()));
    }

    // used for spellchecking after adding a word to user dictionary
    void SpellCheckerService::submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck,
                                          const QStringList &wordsToCheck) {
        if (m_SpellCheckWorker == NULL) { return; }

        std::vector<std::shared_ptr<ISpellCheckItem> > items;
        int length = itemsToCheck.length();

        items.reserve(length);
        auto deleter = [](SpellCheckItem *spi) {
            LOG_INTEGRATION_TESTS << "Delete later for UserDict spelling item";
            spi->disconnect();
            spi->deleteLater();
        };

        for (int i = 0; i < length; ++i) {
            auto *itemToCheck = itemsToCheck.at(i);
            std::shared_ptr<SpellCheckItem> item(new SpellCheckItem(itemToCheck, wordsToCheck),
                                                 deleter);
            itemToCheck->connectSignals(item.get());
            items.emplace_back(std::dynamic_pointer_cast<ISpellCheckItem>(item));
        }

        LOG_INFO << length << "item(s)";

        m_SpellCheckWorker->submitItems(items);
        m_SpellCheckWorker->submitItem(std::shared_ptr<ISpellCheckItem>(new SpellCheckSeparatorItem()));
    }

    void SpellCheckerService::submitKeyword(Common::BasicKeywordsModel *itemToCheck, int keywordIndex) {
        Q_ASSERT(itemToCheck != nullptr);
        LOG_INFO << "index:" << keywordIndex;
        if (m_SpellCheckWorker == NULL) {
            return;
        }

        std::shared_ptr<SpellCheckItem> item(new SpellCheckItem(itemToCheck, Common::SpellCheckFlags::Keywords, keywordIndex),
            [](SpellCheckItem *spi) {
            LOG_INTEGRATION_TESTS << "Delete later for keyword spelling item";
            spi->deleteLater();
        });
        itemToCheck->connectSignals(item.get());
        m_SpellCheckWorker->submitFirst(item);
    }

    QStringList SpellCheckerService::suggestCorrections(const QString &word) const {
        if (m_SpellCheckWorker == NULL) {
            LOG_DEBUG << "Worker is null";
            return QStringList();
        }

        return m_SpellCheckWorker->retrieveCorrections(word);
    }

    void SpellCheckerService::restartWorker() {
        m_RestartRequired = true;
        stopService();
    }

    int SpellCheckerService::getUserDictWordsNumber() {
        if (m_SpellCheckWorker != nullptr) {
            return m_SpellCheckWorker->getUserDictionarySize();
        } else {
            return 0;
        }
    }

    QStringList SpellCheckerService::getUserDictionary() const {
        Q_ASSERT(m_SpellCheckWorker != NULL);
        if (m_SpellCheckWorker != NULL) {
            return m_SpellCheckWorker->getUserDictionary();
        } else {
            return QStringList();
        }
    }

#ifdef INTEGRATION_TESTS
    int SpellCheckerService::getSuggestionsCount() {
        return m_SpellCheckWorker->getSuggestionsCount();
    }
#endif

    void SpellCheckerService::updateUserDictionary(const QStringList &words)
    {
        LOG_INFO << words;
        m_SpellCheckWorker->submitItem(std::shared_ptr<ISpellCheckItem>(new ModifyUserDictItem(words)));
    }

    void SpellCheckerService::cancelCurrentBatch() {
        LOG_DEBUG << "#";

        if (m_SpellCheckWorker == NULL) {
            return;
        }

        m_SpellCheckWorker->cancelCurrentBatch();
    }

    bool SpellCheckerService::hasAnyPending() {
        bool hasPending = false;

        if (m_SpellCheckWorker != NULL) {
            hasPending = m_SpellCheckWorker->hasPendingJobs();
        }

        return hasPending;
    }

    void SpellCheckerService::addWordToUserDictionary(const QString &word) {
        LOG_INFO << word;
        m_SpellCheckWorker->submitItem(std::shared_ptr<ISpellCheckItem>(new ModifyUserDictItem(word)));
    }

    void SpellCheckerService::clearUserDictionary() {
        LOG_DEBUG << "#";
        m_SpellCheckWorker->submitItem(std::shared_ptr<ISpellCheckItem>(new ModifyUserDictItem(true)));
    }

    void SpellCheckerService::workerFinished() {
        LOG_DEBUG << "#";
    }

    void SpellCheckerService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_SpellCheckWorker = NULL;

        if (m_RestartRequired) {
            LOG_INFO << "Restarting worker...";
            startService(std::shared_ptr<Common::ServiceStartParams>());
            m_RestartRequired = false;
        }
    }

    void SpellCheckerService::wordsNumberChangedHandler(int number) {
        LOG_INFO << "Size of dictionary:" << number << "word(s)";
        emit userDictWordsNumberChanged();
    }
}
