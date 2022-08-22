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

#include "mifsa/ota/types/detail_message.h"
#include "mifsa/utils/string.h"
#include <iostream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
std::ostream& operator<<(std::ostream& ostream, const DetailMessage& detailMessage) noexcept
{
    ostream << "{\n";
    ostream << "  [state]: " << Domain::getMrStateStr(detailMessage.state) << "\n";
    ostream << "  [last]: " << Domain::getMrStateStr(detailMessage.last) << "\n";
    ostream << "  [active]: " << (detailMessage.active ? std::string("true") : std::string("false")) << "\n";
    ostream << "  [error]: " << detailMessage.error << "\n";
    ostream << "  [step]: " << (Utils::doubleToString(detailMessage.step) + " %, ") << "\n";
    ostream << "  [progress]: " << (Utils::doubleToString(detailMessage.progress) + " %, ") << "\n";
    ostream << "  [messgae]: " << detailMessage.message << "\n";
    ostream << "  [domains-size]: " << detailMessage.details.size() << "\n";
    ostream << "}";
    return ostream;
}
}

MIFSA_NAMESPACE_END
