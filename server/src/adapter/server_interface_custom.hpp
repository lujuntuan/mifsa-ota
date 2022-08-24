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

#ifndef MIFSA_OTA_SERVER_INTERFACE_CUSTOM_H
#define MIFSA_OTA_SERVER_INTERFACE_CUSTOM_H

#include "mifsa/ota/server_interface.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class ServerInterfaceAdapter : public ServerInterface {
public:
    ServerInterfaceAdapter()
    {
        LOG_WARNING("Rpc adaptation layer is not implemented!");
    }
    ~ServerInterfaceAdapter()
    {
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    void sendControlMessage(const ControlMessage& controlMessage) override
    {
    }
    void sendDetailMessage(const DetailMessage& detailMessage) override
    {
    }
    void setCbReportDomain(const CbDomain& cb) override
    {
    }
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_SERVER_INTERFACE_CUSTOM_H
