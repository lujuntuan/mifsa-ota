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

#ifndef MIFSA_OTA_CLIENT_H
#define MIFSA_OTA_CLIENT_H

#include "mifsa/ota/client_interface.h"
#include "mifsa/ota/config.h"
#include "mifsa/ota/detail_message.h"
#include "mifsa/ota/domain.h"
#include <mifsa/base/singleton.h>
#include <mifsa/module/client.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class MIFSA_EXPORT Client : public ClientProxy<ClientInterface>, public SingletonProxy<Client> {
public:
    using FilePaths = std::vector<std::string>;
    using DeployFunction = std::function<void(const std::string& dir, const FilePaths& filePaths)>;
    using DetailFunction = std::function<void(const DetailMessage& detailMessage, bool stateChanged)>;
    Client(int argc, char** argv);
    ~Client();
    static void registerDomain(const std::string& name, const std::string& guid = "");
    const std::string& name() const;
    const std::string& guid() const;
    const Upgrade& upgrade() const;
    Control control() const;
    bool hasCancelAction() const;
    bool hasStopAction() const;

    bool cancelEnable() const;
    const std::string& version() const;
    const VariantMap& attribute() const;
    const VariantMap& meta() const;
    const VariantMap& packageMeta() const;

    void setCancelEnable(bool cancelEnable);
    void setVersion(const std::string& version);
    void setAttribute(const VariantMap& attribute);
    void setMeta(const VariantMap& meta);

    void subscibeDeploy(const DeployFunction& function);
    void postDeployDone(bool success, int errorCode = 0);
    void postCancelDone(bool success, int errorCode = 0);
    void postDeployProgress(float progress, const std::string& message);

    void subscibeDetail(const DetailFunction& function);
    void postDetailAnswer(Answer answer);

protected:
    virtual void begin() override;
    virtual void end() override;
    virtual void eventChanged(const std::shared_ptr<Event>& event) override;

private:
    bool needDiscovery() const;
    bool hasSubscibeDeploy() const;
    bool hasSubscibeDetail() const;
    void sendHeartbeat();
    void processControlMessage(ControlMessage&& controlMessage);
    void processDetailMessage(DetailMessage&& detailMessage);
    bool checkControlMessageId(uint32_t id) const;
    bool checkDetailMessageId(uint32_t id) const;
    void setDomainState(ClientState state);

private:
    void sendDomainMessage();
    void stopThread(bool force = false);
    void download(const std::string& id, const Files& files);
    void verify(const std::string& id, const Files& files);
    void patch(const FilePaths& patchPaths);
    void deploy(const std::string& id, const Files& files);

private:
    friend class ClientInterfaceAdapter;
    struct ClientHelper* m_clientHelper = nullptr;
};
}

MIFSA_NAMESPACE_END

#define mifsa_ota_client Mifsa::Ota::Client::getInstance()

#endif // MIFSA_OTA_CLIENT_H
