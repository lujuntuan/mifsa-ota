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

#ifndef MIFSA_OTA_WEB_INIT_H
#define MIFSA_OTA_WEB_INIT_H

#include <mifsa/base/define.h>
#include <string>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct MIFSA_EXPORT WebInit final {
    using Token = std::pair<std::string, std::string>;
    WebInit() = default;
    explicit WebInit(const std::string& _url, const std::string& _tenant, const std::string& _id, const Token& _token = Token()) noexcept
        : url(_url)
        , tenant(_tenant)
        , id(_id)
        , token(_token)
    {
    }
    std::string url;
    std::string tenant;
    std::string id;
    Token token;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_WEB_INIT_H
