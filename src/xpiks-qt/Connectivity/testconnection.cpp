/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "testconnection.h"
#include "uploadcontext.h"
#include "ftphelpers.h"
#include <curl/curl.h>
#include "../Common/defines.h"
#include "../Models/proxysettings.h"
#include "../Helpers/stringhelper.h"
#include <uploadcontext.h>

namespace Connectivity {
    static size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data) {
        (void)ptr;
        (void)data;
        /* we are not interested in the headers itself,
         so we only return the size we would have saved ... */
        return (size_t)(size * nmemb);
    }

    ContextValidationResult isContextValid(std::shared_ptr<libxpks::net::UploadContext> &context) {
        bool result = false;
        CURL *curlHandle = NULL;
        QString host = sanitizeHost(context->m_Host);
        const QString &remoteUrl = host;

        // should be called in main() using helper
        //curl_global_init(CURL_GLOBAL_ALL);
        curlHandle = curl_easy_init();

        CURLcode r = CURLE_GOT_NOTHING;

        fillCurlOptions(curlHandle, context, remoteUrl);

        curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_DIRLISTONLY, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, throw_away);
        curl_easy_setopt(curlHandle, CURLOPT_FTP_USE_EPSV, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);

        LOG_DEBUG << "About to check credentials for" << host;
        r = curl_easy_perform(curlHandle);

        result = (CURLE_OK == r);

        if (!result) {
            LOG_INFO << "Credentials checking error:" << curl_easy_strerror(r);
        }

        curl_easy_cleanup(curlHandle);
        // should be called in main()
        //curl_global_cleanup();

        LOG_INFO << "Credentials checking finished" << host;

        ContextValidationResult cvr;
        cvr.m_Host = host;
        cvr.m_Result = result;

        return cvr;
    }
}
