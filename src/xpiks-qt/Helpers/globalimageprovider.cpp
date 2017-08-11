/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "globalimageprovider.h"

namespace Helpers {
    QImage GlobalImageProvider::requestImage(const QString &url, QSize *size, const QSize &requestedSize) {
        QString id;

        if (url.contains(QChar('%'))) {
            QUrl initialUrl(url);
            id = initialUrl.path();
        } else {
            id = url;
        }

        QImage image(id);
        QImage result;

        if (requestedSize.isValid()) {
            result = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else {
            result = image;
        }

        *size = result.size();
        return result;
    }
}
