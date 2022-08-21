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

#ifndef MIFSA_OTA_PROVIDER_H
#define MIFSA_OTA_PROVIDER_H

#include "control_message.h"
#include "detail_message.h"
#include "domain_message.h"
#include <mifsa/module/provider.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class Provider : public ProviderBase {
public:
    using CbReportDomain = std::function<void(const DomainMessage& domainMessage)>;
    virtual void sendControlMessage(const ControlMessage& controlMessage) = 0;
    virtual void sendDetailMessage(const DetailMessage& detailMessage) = 0;
    virtual void setCbReportDomain(const CbReportDomain& cb) = 0;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_PROVIDER_H
