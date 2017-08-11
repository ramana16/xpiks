/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "translationworker.h"
#include <lookupdictionary.h>
#include <string>
#include "../Common/defines.h"
#include "../Helpers/asynccoordinator.h"

namespace Translation {
    TranslationWorker::TranslationWorker(Helpers::AsyncCoordinator *initCoordinator, QObject *parent) :
        QObject(parent),
        m_InitCoordinator(initCoordinator)
    {
    }

    TranslationWorker::~TranslationWorker() {
    }

    void TranslationWorker::selectDictionary(const QString &dictionaryPath) {
        LOG_INFO << dictionaryPath;

        if (m_LookupDictionary->isLoaded()) {
            LOG_INFO << "Unloading old dictionary";
            m_LookupDictionary.reset(new LookupDictionary());
        }

#ifdef Q_OS_WIN
        m_LookupDictionary->setIfoFilePath(dictionaryPath.toStdWString());
#else
        m_LookupDictionary->setIfoFilePath(dictionaryPath.toStdString());
#endif
    }

    bool TranslationWorker::initWorker() {
        LOG_DEBUG << "#";        

        Helpers::AsyncCoordinatorUnlocker unlocker(m_InitCoordinator);
        Q_UNUSED(unlocker);

        m_LookupDictionary.reset(new LookupDictionary());

        return true;
    }

    void TranslationWorker::processOneItem(std::shared_ptr<TranslationQuery> &item) {
        std::string translationData;
        auto &query = item->getQuery();
        QString request = query.simplified().toLower();
        LOG_INFO << "translation request:" << query;

        if (!request.isEmpty()) {
            ensureDictionaryLoaded();
            std::string word = request.toUtf8().toStdString();

            if (m_LookupDictionary->translate(word, translationData)) {
                QString translation = QString::fromUtf8(translationData.c_str());
                item->setTranslation(translation);
            } else {
                item->setFailed();
            }
        } else {
            item->setFailed();
        }
    }

    void TranslationWorker::ensureDictionaryLoaded() {
        Q_ASSERT(m_LookupDictionary);

        if (!m_LookupDictionary->isLoaded()) {
            LOG_INFO << "Loading current dictionary";
            bool result = m_LookupDictionary->loadDictionary();
            if (!result) {
                LOG_WARNING << "Loading dictionary failed!";
            }
        }
    }
}
