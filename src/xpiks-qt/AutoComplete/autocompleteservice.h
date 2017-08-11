/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTOCOMPLETESERVICE_H
#define AUTOCOMPLETESERVICE_H

#include <QObject>
#include <QString>
#include "../Common/iservicebase.h"

namespace KeywordsPresets {
    class IPresetsManager;
}

namespace AutoComplete {
    class AutoCompleteWorker;
    class AutoCompleteModel;

    class AutoCompleteService:
            public QObject,
            public Common::IServiceBase<QString>
    {
        Q_OBJECT
    public:
        AutoCompleteService(AutoCompleteModel *autoCompleteModel, KeywordsPresets::IPresetsManager *presetsManager, QObject *parent = 0);
        virtual ~AutoCompleteService();

        virtual void startService(const std::shared_ptr<Common::ServiceStartParams> &params) override;
        virtual void stopService() override;

        virtual bool isAvailable() const override { return true; }
        virtual bool isBusy() const override;

    protected:
        virtual void submitItem(QString *item) override;
        virtual void submitItem(QString *item, Common::flag_t flags) override;
        virtual void submitItems(const QVector<QString*> &items) override;

    public:
        void restartWorker();

        // copy-by-value on purpose
        void findKeywordCompletions(const QString &prefix, QObject *notifyObject);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject* object);

    signals:
        void cancelAutoCompletion();
        void serviceAvailable(bool afterRestart);

#ifdef INTEGRATION_TESTS
    public:
        AutoCompleteModel *getAutoCompleteModel() const { return m_AutoCompleteModel; }
#endif

    private:
        AutoCompleteWorker *m_AutoCompleteWorker;
        AutoCompleteModel *m_AutoCompleteModel;
        KeywordsPresets::IPresetsManager *m_PresetsManager;
        volatile bool m_RestartRequired;
    };
}

#endif // AUTOCOMPLETESERVICE_H
