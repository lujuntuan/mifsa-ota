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

#ifndef MIFSA_OTA_HAWKBIT_QUEUE_H
#define MIFSA_OTA_HAWKBIT_QUEUE_H

#include "web_queue.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class MIFSA_EXPORT HawkbitQueue final : public WebQueue {
public:
    HawkbitQueue();
    ~HawkbitQueue();

protected:
    virtual bool init(const WebInit& webInit) override;
    virtual bool detect() override;
    virtual bool feedback(const WebFeed& webFeed) override;
    virtual bool transformUpgrade(Upgrade& upgrade, const std::string& jsonString) override;

private:
    struct HawkbitHelper* m_hawkbitHelper = nullptr;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_HAWKBIT_QUEUE_H
