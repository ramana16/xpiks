/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TRANSLATIONSERVICE_H
#define TRANSLATIONSERVICE_H

#include <QObject>
#include "translationmanager.h"
#include "../Common/iservicebase.h"

namespace Translation {
    class TranslationWorker;

    class TranslationService :
            public QObject,
            public Common::IServiceBase<QString>
    {
        Q_OBJECT
    public:
        explicit TranslationService(TranslationManager &manager, QObject *parent = 0);
        virtual ~TranslationService() {}

        virtual void startService(const std::shared_ptr<Common::ServiceStartParams> &params) override;
        virtual void stopService() override;

        virtual bool isAvailable() const override { return true; }
        virtual bool isBusy() const override;

    protected:
        virtual void submitItem(QString *item) override;
        virtual void submitItem(QString *item, Common::flag_t flags) override;
        virtual void submitItems(const std::vector<QString*> &items) override;

    public:
        void selectDictionary(const QString &dictionaryPath);
        void translate(const QString &what);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject* object);

    private:
        TranslationManager &m_TranslationManager;
        TranslationWorker *m_TranslationWorker;
        volatile bool m_RestartRequired;
    };
}

#endif // TRANSLATIONSERVICE_H
