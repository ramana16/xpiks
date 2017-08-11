/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXPORTINFO_H
#define EXPORTINFO_H

#include <QString>
#include <QSize>

namespace Models {
    class ExportInfo {
    public:
        ExportInfo() :
            m_MustSaveOriginal(false) {}
        ~ExportInfo() {}

    public:
        bool getMustSaveOriginal() const { return m_MustSaveOriginal; }
        void setMustSaveOriginal(bool value) { m_MustSaveOriginal = value; }

    private:
        bool m_MustSaveOriginal;
    };
}

#endif // EXPORTINFO_H
