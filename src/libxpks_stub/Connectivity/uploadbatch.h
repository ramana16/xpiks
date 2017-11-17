/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UPLOADBATCH_H
#define UPLOADBATCH_H

#include <memory>
#include <QStringList>
#include "uploadcontext.h"

namespace libxpks {
    namespace net {
        class UploadBatch {
        public:
            UploadBatch(const std::shared_ptr<UploadContext> &context, const QStringList &filesList):
                m_FilesList(filesList),
                m_UploadContext(context)
            {}

            virtual ~UploadBatch() { }

        public:
            const QStringList &getFilesToUpload() const { return m_FilesList; }
            const std::shared_ptr<UploadContext> &getContext() const { return m_UploadContext; }

        private:
            QStringList m_FilesList;
            std::shared_ptr<UploadContext> m_UploadContext;
        };
    }
}

#endif // UPLOADBATCH_H
