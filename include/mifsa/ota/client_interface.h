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

#ifndef MIFSA_OTA_CLIENT_INTERFACE_H
#define MIFSA_OTA_CLIENT_INTERFACE_H

#include "control_message.h"
#include "detail_message.h"
#include "domain_message.h"
#include <mifsa/module/client.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class ClientInterface : public ClientInterfaceBase {
public:
    using CbControlMessage = std::function<void(const ControlMessage& controlMessage)>;
    using CbDetailMessage = std::function<void(const DetailMessage& detailMessage)>;
    virtual void setCbControlMessage(CbControlMessage cb) = 0;
    virtual void setCbDetailMessage(CbDetailMessage cb) = 0;
    virtual bool sendDomain(const DomainMessage& domainMessage) = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CLIENT_INTERFACE_H
