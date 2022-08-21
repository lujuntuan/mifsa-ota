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

#include <mifsa/ota/platform.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class PlatformLinux : public Platform {
public:
    virtual std::string getNmea() override
    {
        return "empty";
    }
};

MIFSA_CREATE_PLATFORM(PlatformLinux, 1, 0);
}

MIFSA_NAMESPACE_END
