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

#ifndef MIFSA_OTA_CLIENT_INTERFACE_VSOMEIP_H
#define MIFSA_OTA_CLIENT_INTERFACE_VSOMEIP_H

#ifdef MIFSA_SUPPORT_VSOMEIP

#include "mifsa/ota/client.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa/ota/interfacesProxy.hpp>

using namespace v1_0::commonapi::mifsa::ota;

MIFSA_NAMESPACE_BEGIN

CommonAPI::CallInfo _callInfo(5000);

namespace Ota {
static Package _getPackage(const interfaces::Package& ci_package)
{
    Package package;
    package.domain = ci_package.getDomain();
    package.part = ci_package.getPart();
    package.version = ci_package.getVersion_();
    package.meta = Variant::readJson(ci_package.getMeta());
    for (const auto& ci_file : ci_package.getFiles()) {
        File file;
        file.domain = ci_file.getDomain();
        file.name = ci_file.getName();
        file.url = ci_file.getUrl();
        file.size = ci_file.getSize();
        file.md5 = ci_file.getMd5();
        file.sha1 = ci_file.getSha1();
        file.sha256 = ci_file.getSha256();
        package.files.push_back(std::move(file));
    }
    return package;
}

static ControlMessage _getControlMessage(const interfaces::ControlMessage& ci_controlMessage)
{
    uint32_t id = ci_controlMessage.getId();
    Control control = (Control)ci_controlMessage.getControl().value_;
    Upgrade upgrade;
    upgrade.id = ci_controlMessage.getUpgrade().getId();
    upgrade.download = (Upgrade::Method)ci_controlMessage.getUpgrade().getDownload().value_;
    upgrade.deploy = (Upgrade::Method)ci_controlMessage.getUpgrade().getDeploy().value_;
    upgrade.maintenance = ci_controlMessage.getUpgrade().getMaintenance();
    for (const auto& ci_package : ci_controlMessage.getUpgrade().getPackages()) {
        const Package& package = _getPackage(ci_package);
        upgrade.packages.push_back(std::move(package));
    }
    Depends depends;
    for (const auto& ci_depend : ci_controlMessage.getDepends()) {
        depends.push_back(std::move(ci_depend.getData()));
    }
    return ControlMessage(id, control, std::move(upgrade), std::move(depends));
}

static DetailMessage _getDetailMessage(const interfaces::DetailMessage& ci_detailMessage)
{
    uint32_t id = ci_detailMessage.getId();
    ServerState state = (ServerState)ci_detailMessage.getState_().value_;
    ServerState last = (ServerState)ci_detailMessage.getLast().value_;
    bool active = ci_detailMessage.getActive();
    int error = ci_detailMessage.getError_();
    float step = ci_detailMessage.getStep();
    float progress = ci_detailMessage.getProgress();
    const std::string& message = ci_detailMessage.getMessage();
    Details details;
    for (const auto& ci_detail : ci_detailMessage.getDetails()) {
        Domain domain(ci_detail.getDomain().getName(), ci_detail.getDomain().getGuid());
        domain.state = (ClientState)ci_detail.getDomain().getState_().value_;
        domain.last = (ClientState)ci_detail.getDomain().getLast().value_;
        domain.watcher = ci_detail.getDomain().getWatcher();
        domain.error = ci_detail.getDomain().getError_();
        domain.version = ci_detail.getDomain().getVersion_();
        domain.attribute = Variant::readJson(ci_detail.getDomain().getAttribute_());
        domain.meta = Variant::readJson(ci_detail.getDomain().getMeta());
        domain.progress = ci_detail.getDomain().getProgress();
        domain.message = ci_detail.getDomain().getMessage();
        domain.answer = (Answer)ci_detail.getDomain().getAnswer().value_;
        Detail detail(std::move(domain));
        const Package& package = _getPackage(ci_detail.getPackage_());
        detail.package = package;
        for (const auto& ci_transfer : ci_detail.getTransfers()) {
            Transfer transfer;
            transfer.domain = ci_transfer.getDomain();
            transfer.name = ci_transfer.getName();
            transfer.progress = ci_transfer.getProgress();
            transfer.speed = ci_transfer.getSpeed();
            transfer.total = ci_transfer.getTotal();
            transfer.current = ci_transfer.getCurrent();
            transfer.pass = ci_transfer.getPass();
            transfer.left = ci_transfer.getLeft();
            detail.transfers.push_back(std::move(transfer));
        }
        detail.progress = ci_detail.getProgress();
        if (ci_detail.getDeploy() > 0) {
            detail.deploy.start(Elapsed::current() - ci_detail.getDeploy());
        }
        details.push_back(std::move(detail));
    }
    return DetailMessage(id, state, last, active, error, step, progress, std::move(message), std::move(details));
}

static interfaces::DomainMessage _getDomainMessage(const DomainMessage& domainMessage)
{
    interfaces::DomainMessage ci_domainMessage;
    interfaces::Domain ci_domain;
    ci_domain.setName(domainMessage.domain.name);
    ci_domain.setGuid(domainMessage.domain.guid);
    ci_domain.setState_((interfaces::ClientState::Literal)domainMessage.domain.state);
    ci_domain.setLast((interfaces::ClientState::Literal)domainMessage.domain.last);
    ci_domain.setWatcher(domainMessage.domain.watcher);
    ci_domain.setError_(domainMessage.domain.error);
    ci_domain.setVersion_(domainMessage.domain.version);
    ci_domain.setAttribute_(domainMessage.domain.attribute.toJson());
    ci_domain.setMeta(domainMessage.domain.meta.toJson());
    ci_domain.setProgress(domainMessage.domain.progress);
    ci_domain.setMessage(domainMessage.domain.message);
    ci_domain.setAnswer((interfaces::Answer::Literal)domainMessage.domain.answer);
    ci_domainMessage.setDomain(std::move(ci_domain));
    ci_domainMessage.setDiscovery(domainMessage.discovery);
    return ci_domainMessage;
}
class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_client.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        m_commonApiProxy = CommonAPI::Runtime::get()->buildProxy<interfacesProxy>("local", "commonapi.mifsa.ota.interfaces", "mifsa_ota_client");
        m_commonApiProxy->getProxyStatusEvent().subscribe([this](const CommonAPI::AvailabilityStatus& status) {
            if (status == CommonAPI::AvailabilityStatus::AVAILABLE) {
                _cbConnected(true);
            } else {
                _cbConnected(false);
            }
        });
        m_commonApiProxy->getDispatchControlMessageEvent().subscribe([this](const interfaces::ControlMessage& ci_controlMessage) {
            if (mifsa_ota_client->checkControlMessageId(ci_controlMessage.getId())) {
                return;
            }
            mifsa_ota_client->processControlMessage(_getControlMessage(ci_controlMessage));
        });
        if (mifsa_ota_client->hasSubscibeDetail()) {
            m_commonApiProxy->getDispatchDetailMessageEvent().subscribe([this](const interfaces::DetailMessage& ci_detailMessage) {
                if (mifsa_ota_client->checkDetailMessageId(ci_detailMessage.getId())) {
                    return;
                }
                mifsa_ota_client->processDetailMessage(_getDetailMessage(ci_detailMessage));
            });
        }
    }
    ~ClientInterfaceAdapter()
    {
        m_commonApiProxy.reset();
        CommonAPI::Runtime::get().reset();
    }
    virtual std::string version() override
    {
        return MIFSA_OTA_VERSION;
    }
    virtual bool connected() override
    {
        return m_commonApiProxy->isAvailable();
    }
    virtual void setCbControlMessage(CbControlMessage cb) override
    {
        m_cbControlMessage = cb;
    }
    virtual void setCbDetailMessage(CbDetailMessage cb) override
    {
        m_cbDetailMessage = cb;
    }
    virtual bool sendDomain(const DomainMessage& domainMessage) override
    {
        if (!m_commonApiProxy->isAvailable()) {
            return false;
        }
        CommonAPI::CallStatus callStatus;
        m_commonApiProxy->invokeDomainMessage(_getDomainMessage(domainMessage), callStatus);
        return true;
    }

private:
    std::shared_ptr<interfacesProxy<>> m_commonApiProxy;
    CbControlMessage m_cbControlMessage;
    CbDetailMessage m_cbDetailMessage;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_CLIENT_INTERFACE_VSOMEIP_H
