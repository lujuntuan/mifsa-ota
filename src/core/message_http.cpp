/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#include "mifsa/ota/setting.h"
#if (defined(MIFSA_SERVER_TYPE) && defined(MIFSA_USE_MESSAGE_HTTP))
#include "config_http.h"
#include "core.h"
#include "helper.h"
#include "importlib/httplib.hpp"
#include <mifsa/base/log.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Core {
    Status getMessage(const std::string& url, const std::string& path, std::string& body, const VariantMap& config,
        const Token& token)
    {
        StatusHelper statusHelper;
        if (path.empty()) {
            statusHelper.throwError(401);
            LOG_WARNING("http get path is empty");
            return statusHelper.status;
        }
        httplib::Client client(url);
        httplib::Headers headers = {
            { "Accept", "application/hal+json" }
        };
        if (!token.first.empty()) {
            headers.insert(
                { "Authorization", token.first + " " + token.second });
        }
        if (!loadClientConfig(client, config)) {
            statusHelper.throwError(402);
            LOG_WARNING("load client config error");
            return statusHelper.status;
        }
        auto res = client.Get(path.c_str(), headers);
        if (!res) {
            statusHelper.throwError(403);
            LOG_WARNING("http get json error, code=", res.error());
            return statusHelper.status;
        }
        body = res->body;
        if (res->body.empty()) {
            statusHelper.throwError(404);
            LOG_WARNING("http get res is empty");
            return statusHelper.status;
        }
        return statusHelper.status;
    }

    Status putMessage(const std::string& url, const std::string& path, const std::string& body, const VariantMap& config, const Token& token)
    {
        StatusHelper statusHelper;
        if (path.empty()) {
            statusHelper.throwError(501, "http put path is empty");
            return statusHelper.status;
        }
        httplib::Client client(url);
        httplib::Headers headers = {
            { "Accept", "application/hal+json" }
        };
        if (!token.first.empty()) {
            headers.insert(
                { "Authorization", token.first + " " + token.second });
        }
        if (!loadClientConfig(client, config)) {
            statusHelper.throwError(502);
            LOG_WARNING("load client config error");
            return statusHelper.status;
        }
        auto res = client.Put(path.c_str(), headers, body, "application/json;charset=UTF-8");
        if (!res) {
            statusHelper.throwError(503);
            LOG_WARNING("http put json error");
            return statusHelper.status;
        }
        return statusHelper.status;
    }

    Status postMessage(const std::string& url, const std::string& path, const std::string& body, const VariantMap& config, const Token& token)
    {
        StatusHelper statusHelper;
        if (path.empty()) {
            statusHelper.throwError(601, "http post path is empty");
            return statusHelper.status;
        }
        httplib::Client client(url);
        httplib::Headers headers = {
            { "Accept", "application/hal+json" }
        };
        if (!token.first.empty()) {
            headers.insert(
                { "Authorization", token.first + " " + token.second });
        }
        if (!loadClientConfig(client, config)) {
            statusHelper.throwError(602);
            LOG_WARNING("load client config error");
            return statusHelper.status;
        }
        auto res = client.Post(path.c_str(), headers, body, "application/json;charset=UTF-8");
        if (!res) {
            statusHelper.throwError(603);
            LOG_WARNING("http post json error");
            return statusHelper.status;
        }
        return statusHelper.status;
    }
}
}

MIFSA_NAMESPACE_END

#endif
