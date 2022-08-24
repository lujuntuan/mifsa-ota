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

#ifndef MIFSA_OTA_CLIENT_INTERFACE_CUSTOM_H
#define MIFSA_OTA_CLIENT_INTERFACE_CUSTOM_H

#include "mifsa/ota/client_interface.h"
#include "mifsa/ota/config.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        LOG_WARNING("Rpc adaptation layer is not implemented!");
    }
    ~ClientInterfaceAdapter()
    {
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual std::string version() override
    {
        return MIFSA_OTA_VERSION;
    }
    virtual bool connected() override
    {
        return false;
    }
    virtual void setCbControlMessage(CbControlMessage cb) override
    {
    }
    virtual void setCbDetailMessage(CbDetailMessage cb) override
    {
    }
    virtual bool sendDomain(const DomainMessage& domainMessage) override
    {
        return false;
    }
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CLIENT_INTERFACE_CUSTOM_H
