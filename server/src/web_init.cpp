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

#include "web_init.h"
#include <iostream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
std::ostream& operator<<(std::ostream& ostream, const WebInit& webInit) noexcept
{
    ostream << "{\n";
    ostream << "  [url]: " << webInit.url << "\n";
    ostream << "  [tenant]: " << webInit.tenant << "\n";
    ostream << "  [id]: " << webInit.id << "\n";
    ostream << "  [token]: " << webInit.token.first << " " << webInit.token.second << "\n";
    ostream << "}";
    return ostream;
}
}

MIFSA_NAMESPACE_END
