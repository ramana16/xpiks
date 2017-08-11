/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INITIALIZEDICTIONARIESJOBITEM_H
#define INITIALIZEDICTIONARIESJOBITEM_H

#include "imaintenanceitem.h"

namespace Translation {
    class TranslationManager;
}

namespace Helpers {
    class AsyncCoordinator;
}

namespace Maintenance {
    class InitializeDictionariesJobItem : public IMaintenanceItem
    {
    public:
        InitializeDictionariesJobItem(Translation::TranslationManager *translationManager, Helpers::AsyncCoordinator *initCoordinator);

    public:
        virtual void processJob() override;

    private:
        void doInitializeDictionaries();

    private:
        Translation::TranslationManager *m_TranslationManager;
        Helpers::AsyncCoordinator *m_InitCoordinator;
    };
}

#endif // INITIALIZEDICTIONARIESJOBITEM_H
