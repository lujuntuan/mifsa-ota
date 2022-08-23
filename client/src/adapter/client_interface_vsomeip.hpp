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

#include "mifsa/ota/client.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/mifsa_ota_idl/CommonProxy.hpp>

using namespace v1_0;

MIFSA_NAMESPACE_BEGIN

namespace Ota {
static Package _getPackage(const mifsa_ota_idl::Common::Package& t_package)
{
    Package package;
    package.domain = t_package.getDomain();
    package.part = t_package.getPart();
    package.version = t_package.getVersion_();
    package.meta = Variant::readJson(t_package.getMeta());
    for (const auto& t_file : t_package.getFiles()) {
        File file;
        file.domain = t_file.getDomain();
        file.name = t_file.getName();
        file.url = t_file.getUrl();
        file.size = t_file.getSize();
        file.md5 = t_file.getMd5();
        file.sha1 = t_file.getSha1();
        file.sha256 = t_file.getSha256();
        package.files.push_back(std::move(file));
    }
    return package;
}

static ControlMessage _getControlMessage(const mifsa_ota_idl::Common::ControlMessage& t_controlMessage)
{
    ControlMessage controlMessage;
    controlMessage.id = t_controlMessage.getId();
    controlMessage.control = (Control)t_controlMessage.getControl().value_;
    controlMessage.upgrade.id = t_controlMessage.getUpgrade().getId();
    controlMessage.upgrade.download = (Upgrade::Method)t_controlMessage.getUpgrade().getDownload().value_;
    controlMessage.upgrade.deploy = (Upgrade::Method)t_controlMessage.getUpgrade().getDeploy().value_;
    controlMessage.upgrade.maintenance = t_controlMessage.getUpgrade().getMaintenance();
    for (const auto& t_package : t_controlMessage.getUpgrade().getPackages()) {
        const Package& package = _getPackage(t_package);
        controlMessage.upgrade.packages.push_back(std::move(package));
    }
    for (const auto& t_depend : t_controlMessage.getDepends()) {
        controlMessage.depends.push_back(std::move(t_depend.getData()));
    }
    return controlMessage;
}

static DetailMessage _getDetailMessage(const mifsa_ota_idl::Common::DetailMessage& t_detailMessage)
{
    DetailMessage detailMessage;
    detailMessage.id = t_detailMessage.getId();
    detailMessage.state = (ServerState)t_detailMessage.getState_().value_;
    detailMessage.last = (ServerState)t_detailMessage.getLast().value_;
    detailMessage.active = t_detailMessage.getActive();
    detailMessage.error = t_detailMessage.getError_();
    detailMessage.step = t_detailMessage.getStep();
    detailMessage.progress = t_detailMessage.getProgress();
    detailMessage.message = t_detailMessage.getMessage();
    for (const auto& t_detail : t_detailMessage.getDetails()) {
        Domain domain(t_detail.getDomain().getName(), t_detail.getDomain().getGuid());
        domain.state = (ClientState)t_detail.getDomain().getState_().value_;
        domain.last = (ClientState)t_detail.getDomain().getLast().value_;
        domain.watcher = t_detail.getDomain().getWatcher();
        domain.error = t_detail.getDomain().getError_();
        domain.version = t_detail.getDomain().getVersion_();
        domain.attribute = Variant::readJson(t_detail.getDomain().getAttribute_());
        domain.meta = Variant::readJson(t_detail.getDomain().getMeta());
        domain.progress = t_detail.getDomain().getProgress();
        domain.message = t_detail.getDomain().getMessage();
        domain.answer = (Answer)t_detail.getDomain().getAnswer().value_;
        Detail detail(std::move(domain));
        const auto& package = _getPackage(t_detail.getPackage_());
        detail.package = package;
        for (const auto& t_transfer : t_detail.getTransfers()) {
            Transfer transfer;
            transfer.domain = t_transfer.getDomain();
            transfer.name = t_transfer.getName();
            transfer.progress = t_transfer.getProgress();
            transfer.speed = t_transfer.getSpeed();
            transfer.total = t_transfer.getTotal();
            transfer.current = t_transfer.getCurrent();
            transfer.pass = t_transfer.getPass();
            transfer.left = t_transfer.getLeft();
            detail.transfers.push_back(std::move(transfer));
        }
        detail.progress = t_detail.getProgress();
        if (t_detail.getDeploy() > 0) {
            detail.deploy.start(Elapsed::current() - t_detail.getDeploy());
        }
        detailMessage.details.push_back(std::move(detail));
    }
    return detailMessage;
}

static mifsa_ota_idl::Common::DomainMessage _getDomainMessage(const DomainMessage& domainMessage)
{
    mifsa_ota_idl::Common::DomainMessage t_domainMessage;
    mifsa_ota_idl::Common::Domain t_domain;
    t_domain.setName(domainMessage.domain.name);
    t_domain.setGuid(domainMessage.domain.guid);
    t_domain.setState_((mifsa_ota_idl::Common::ClientState::Literal)domainMessage.domain.state);
    t_domain.setLast((mifsa_ota_idl::Common::ClientState::Literal)domainMessage.domain.last);
    t_domain.setWatcher(domainMessage.domain.watcher);
    t_domain.setError_(domainMessage.domain.error);
    t_domain.setVersion_(domainMessage.domain.version);
    t_domain.setAttribute_(domainMessage.domain.attribute.toJson());
    t_domain.setMeta(domainMessage.domain.meta.toJson());
    t_domain.setProgress(domainMessage.domain.progress);
    t_domain.setMessage(domainMessage.domain.message);
    t_domain.setAnswer((mifsa_ota_idl::Common::Answer::Literal)domainMessage.domain.answer);
    t_domainMessage.setDomain(std::move(t_domain));
    t_domainMessage.setDiscovery(domainMessage.discovery);
    return t_domainMessage;
}

class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_client.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        m_commonApiProxy = CommonAPI::Runtime::get()->buildProxy<mifsa_ota_idl::CommonProxy>("local", "mifsa_ota_idl.Common", "mifsa_ota_client");
        m_commonApiProxy->getProxyStatusEvent().subscribe([this](const CommonAPI::AvailabilityStatus& status) {
            if (status == CommonAPI::AvailabilityStatus::AVAILABLE) {
                cbConnected(true);
            } else {
                cbConnected(false);
            }
        });
        m_commonApiProxy->getDispatchControlMessageEvent().subscribe([this](const mifsa_ota_idl::Common::ControlMessage& t_controlMessage) {
            if (checkControlMessageId && checkControlMessageId(t_controlMessage.getId())) {
                return;
            }
            const auto& controlMessage = _getControlMessage(t_controlMessage);
            if (m_cbControlMessage) {
                m_cbControlMessage(controlMessage);
            }
        });
    }
    ~ClientInterfaceAdapter()
    {
        m_commonApiProxy.reset();
        CommonAPI::Runtime::get().reset();
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
        return m_commonApiProxy->isAvailable();
    }
    virtual void setCbControlMessage(CbControlMessage cb) override
    {
        m_cbControlMessage = cb;
    }
    virtual void setCbDetailMessage(CbDetailMessage cb) override
    {
        m_cbDetailMessage = cb;
        m_commonApiProxy->getDispatchDetailMessageEvent().subscribe([this](const mifsa_ota_idl::Common::DetailMessage& t_detailMessage) {
            if (checkDetailMessageId && checkDetailMessageId(t_detailMessage.getId())) {
                return;
            }
            const auto& detailMessage = _getDetailMessage(t_detailMessage);
            if (m_cbDetailMessage) {
                m_cbDetailMessage(detailMessage);
            }
        });
    }
    virtual bool sendDomain(const DomainMessage& domainMessage) override
    {
        if (!connected()) {
            return false;
        }
        CommonAPI::CallStatus callStatus;
        const auto& t_domainMessage = _getDomainMessage(domainMessage);
        m_commonApiProxy->invokeDomainMessage(t_domainMessage, callStatus);
        return true;
    }

private:
    std::shared_ptr<mifsa_ota_idl::CommonProxy<>> m_commonApiProxy;
    CbControlMessage m_cbControlMessage;
    CbDetailMessage m_cbDetailMessage;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CLIENT_INTERFACE_VSOMEIP_H
