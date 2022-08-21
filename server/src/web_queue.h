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

#ifndef MIFSA_OTA_WEB_QUEUE_H
#define MIFSA_OTA_WEB_QUEUE_H

#include "mifsa/ota/upgrade.h"
#include "web_feed.h"
#include "web_init.h"
#include <mifsa/base/queue.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class MIFSA_EXPORT WebQueue : public Queue {
public:
    enum State {
        WEB_UNKNOWN = -1,
        WEB_IDLE,
        WEB_DOWNLOAD,
        WEB_VERIFY,
        WEB_DISTRIBUTE,
    };
    WebQueue();
    ~WebQueue();
    State state() const;

protected:
    const std::string& localUrl() const;
    void setCheckInterval(int time);

private:
    virtual void begin() override;
    virtual void end() override;
    virtual void eventChanged(const std::shared_ptr<Event>& event) override;

    virtual bool init(const WebInit& webInit) = 0;
    virtual bool detect() = 0;
    virtual bool feedback(const WebFeed& webFeed) = 0;
    virtual bool transformUpgrade(Upgrade& upgrade, const std::string& jsonString) = 0;

protected:
    void postError(int errorCode);
    void postIdle();
    void postUpgrade(Upgrade&& upgrade);
    void postCancel(std::string&& id);
    void setCheckTimerInterval(int interval);

private:
    void checkUpgrade();
    void removeCache(bool all);

private:
    void stopThread(bool force = false);
    void transformFiles(std::string& id, Files& files, bool webInstead = false);
    void download(const std::string& id, const Files& files);
    void verify(const std::string& id, const Files& files);
    void distribute(const std::string& id, const Files& files);

private:
    struct WebHelper* m_webHelper = nullptr;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_WEB_QUEUE_H
