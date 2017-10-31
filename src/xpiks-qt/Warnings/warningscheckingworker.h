/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WARNINGSCHECKINGWORKER_H
#define WARNINGSCHECKINGWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "iwarningsitem.h"
#include "../Common/flags.h"

namespace Warnings {
    class WarningsSettingsModel;
    class WarningsItem;

    class WarningsCheckingWorker:
        public QObject, public Common::ItemProcessingWorker<IWarningsItem>
    {
    Q_OBJECT

    public:
        WarningsCheckingWorker(WarningsSettingsModel *warningsSettingsModel, QObject *parent=0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItemEx(Common::flag_t flags, std::shared_ptr<IWarningsItem> &item) override;
        virtual void processOneItem(std::shared_ptr<IWarningsItem> &item) override;

    private:
        void processWarningsItem(std::shared_ptr<WarningsItem> &item);
        void initValuesFromSettings();

    protected:
        virtual void onQueueIsEmpty() override { /* Notify only on batches */ /* emit queueIsEmpty(); */ }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        Common::flag_t checkDimensions(std::shared_ptr<WarningsItem> &wi) const;
        Common::flag_t checkKeywords(std::shared_ptr<WarningsItem> &wi) const;
        Common::flag_t checkDescription(std::shared_ptr<WarningsItem> &wi) const;
        Common::flag_t checkTitle(std::shared_ptr<WarningsItem> &wi) const;
        Common::flag_t checkSpelling(std::shared_ptr<WarningsItem> &wi) const;
        Common::flag_t checkDuplicates(std::shared_ptr<WarningsItem> &wi) const;

    private:
        WarningsSettingsModel *m_WarningsSettingsModel;
    };
}

#endif // WARNINGSCHECKINGWORKER_H
