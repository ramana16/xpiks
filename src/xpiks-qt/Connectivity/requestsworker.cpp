/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "requestsworker.h"
#include "simplecurlrequest.h"

namespace Connectivity {
    RequestsWorker::RequestsWorker(QObject *parent) : QObject(parent)
    {
    }

    bool RequestsWorker::initWorker() {
        LOG_DEBUG << "#";
        return true;
    }

    void RequestsWorker::processOneItem(std::shared_ptr<ConnectivityRequest> &item) {
        auto &url = item->getURL();
        LOG_INFO << "Request:" << url;

        SimpleCurlRequest request(url);
        request.setProxySettings(item->getProxySettings());

        bool success = request.sendRequestSync();
        if (success) {
            item->setResponse(request.getResponseData());
        } else {
            LOG_WARNING << "Failed to get" << url;
        }
    }
}
