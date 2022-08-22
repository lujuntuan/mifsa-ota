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

#ifndef MIFSA_OTA_SERVER_H
#define MIFSA_OTA_SERVER_H

#include "mifsa/ota/config.h"
#include "mifsa/ota/server_interface.h"
#include <mifsa/base/singleton.h>
#include <mifsa/module/server.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class Server : public ServerProxy<ServerInterface>, public SingletonProxy<Server> {

public:
    Server(int argc, char** argv);
    ~Server();
    const Upgrade& upgrade() const;
    const Details& details() const;
    const Depends& depends() const;
    const std::string& cancelId() const;
    const std::string& message() const;
    const VariantMap attributes() const;
    ServerState state() const;
    ServerState lastState() const;
    Control control() const;
    int errorCode() const;
    bool isActive() const;
    float step() const;
    float progress() const;
    void processDomainMessage(const DomainMessage& domainMessage);

protected:
    virtual void begin() override;
    virtual void end() override;
    virtual void eventChanged(const std::shared_ptr<Event>& event) override;

private:
    std::vector<std::string> getWebFeedDetails() const;
    std::pair<int, int> getWebFeedProgress() const;
    void configState(bool isLastError = false);
    void configAnswer(Answer answer);
    void setState(ServerState state);
    void readState();
    void resume();
    void idle();
    void pending();
    void download();
    void deploy();
    void cancel();
    void feedback(bool finished, int error = 0);
    void sendControlMessage(Control control, bool cache = false);
    void sendDetailMessage(bool cache = false);
    void processDomains();

private:
    struct ServerHelper* m_serverHelper = nullptr;
    friend class ServerInterfaceAdapter;
    friend class WebQueue;
};
}

MIFSA_NAMESPACE_END

#define mifsa_ota_server Mifsa::Ota::Server::getInstance()

#endif // MIFSA_OTA_SERVER_H
