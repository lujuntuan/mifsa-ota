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

#include "mifsa/ota/control_message.h"
#include "mifsa/utils/string.h"
#include <iostream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
std::ostream& operator<<(std::ostream& ostream, const ControlMessage& controlMessage) noexcept
{
    return ostream;
}
}

MIFSA_NAMESPACE_END
