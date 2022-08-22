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

#ifndef MIFSA_SERVER_OTA_INTERFACE_H
#define MIFSA_SERVER_OTA_INTERFACE_H

#include "types/control_message.h"
#include "types/detail_message.h"
#include "types/domain_message.h"
#include <mifsa/module/server.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class ServerInterface : public ServerInterfaceBase {
public:
    using CbDomain = std::function<void(const DomainMessage& domainMessage)>;
    virtual void sendControlMessage(const ControlMessage& controlMessage) = 0;
    virtual void sendDetailMessage(const DetailMessage& detailMessage) = 0;
    virtual void setCbReportDomain(const CbDomain& cb) = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_SERVER_OTA_INTERFACE_H
