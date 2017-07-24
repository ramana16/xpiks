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

#ifndef WARNINGSCHECKINGWORKER_H
#define WARNINGSCHECKINGWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "iwarningsitem.h"

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
        virtual void processOneItem(std::shared_ptr<IWarningsItem> &item) override;

    private:
        void processWarningsItem(std::shared_ptr<WarningsItem> &item);
        void initValuesFromSettings();

    protected:
        virtual void notifyQueueIsEmpty() override { /* Notify only on batches */ /* emit queueIsEmpty(); */ }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        Common::WarningFlags checkDimensions(std::shared_ptr<WarningsItem> &wi) const;
        Common::WarningFlags checkKeywords(std::shared_ptr<WarningsItem> &wi) const;
        Common::WarningFlags checkDescription(std::shared_ptr<WarningsItem> &wi) const;
        Common::WarningFlags checkTitle(std::shared_ptr<WarningsItem> &wi) const;
        Common::WarningFlags checkSpelling(std::shared_ptr<WarningsItem> &wi) const;
        Common::WarningFlags checkDuplicates(std::shared_ptr<WarningsItem> &wi) const;

    private:
        WarningsSettingsModel *m_WarningsSettingsModel;
    };
}

#endif // WARNINGSCHECKINGWORKER_H
