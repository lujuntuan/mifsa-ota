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

#ifndef MIFSA_OTA_PLATFORM_H
#define MIFSA_OTA_PLATFORM_H

#include <mifsa/module/platform.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class Platform : public PlatformBase {
    MIFSA_PLUGIN_REGISTER("mifsa_ota_platform")
public:
    virtual std::string getNmea() = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_PLATFORM_H
