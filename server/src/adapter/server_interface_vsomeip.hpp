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

#ifndef MIFSA_OTA_SERVER_INTERFACE_VSOMEIP_H
#define MIFSA_OTA_SERVER_INTERFACE_VSOMEIP_H

#ifdef MIFSA_SUPPORT_VSOMEIP

#include "server.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa/ota/interfacesStubDefault.hpp>

using namespace v1_0::commonapi::mifsa::ota;

MIFSA_NAMESPACE_BEGIN
namespace Ota {
static interfaces::Package _getPackage(const Package& package)
{
    interfaces::Package ci_package;
    ci_package.setDomain(package.domain);
    ci_package.setPart(package.part);
    ci_package.setVersion_(package.version);
    ci_package.setMeta(package.meta.toJson());
    interfaces::Files ci_files;
    for (const File& file : package.files) {
        interfaces::File ci_file;
        ci_file.setDomain(file.domain);
        ci_file.setName(file.name);
        ci_file.setUrl(file.url);
        ci_file.setSize(file.size);
        ci_file.setMd5(file.md5);
        ci_file.setSha1(file.sha1);
        ci_file.setSha256(file.sha256);
        ci_files.push_back(std::move(ci_file));
    }
    ci_package.setFiles(ci_files);
    return ci_package;
}

static interfaces::ControlMessage _getControlMessage(const ControlMessage& controlMessage)
{
    interfaces::ControlMessage ci_controlMessage;
    ci_controlMessage.setId(controlMessage.id);
    ci_controlMessage.setControl((interfaces::Control::Literal)controlMessage.control);
    interfaces::Upgrade ci_upgrade;
    ci_upgrade.setId(controlMessage.upgrade.id);
    ci_upgrade.setDownload((interfaces::Method::Literal)controlMessage.upgrade.download);
    ci_upgrade.setDeploy((interfaces::Method::Literal)controlMessage.upgrade.deploy);
    ci_upgrade.setMaintenance(controlMessage.upgrade.maintenance);
    interfaces::Packages ci_packages;
    for (const Package& package : controlMessage.upgrade.packages) {
        const interfaces::Package& ci_package = _getPackage(package);
        ci_packages.push_back(std::move(ci_package));
    }
    ci_upgrade.setPackages(std::move(ci_packages));
    ci_controlMessage.setUpgrade(std::move(ci_upgrade));
    interfaces::Depends ci_depends;
    for (const auto& str : controlMessage.depends) {
        interfaces::Depend ci_depend;
        ci_depend.setData(str);
        ci_depends.push_back(std::move(ci_depend));
    }
    ci_controlMessage.setDepends(ci_depends);
    return ci_controlMessage;
}

static interfaces::DetailMessage _getDetailMessage(const DetailMessage& detailMessage)
{
    interfaces::DetailMessage ci_detailMessage;
    ci_detailMessage.setId(detailMessage.id);
    ci_detailMessage.setState_((interfaces::ServerState::Literal)detailMessage.state);
    ci_detailMessage.setLast((interfaces::ServerState::Literal)detailMessage.last);
    ci_detailMessage.setActive(detailMessage.active);
    ci_detailMessage.setError_(detailMessage.error);
    ci_detailMessage.setStep(detailMessage.step);
    ci_detailMessage.setProgress(detailMessage.progress);
    ci_detailMessage.setMessage(detailMessage.message);
    interfaces::Details ci_details;
    for (const auto& d : detailMessage.details) {
        interfaces::Detail ci_detail;
        interfaces::Domain ci_domain;
        ci_domain.setName(d.domain.name);
        ci_domain.setGuid(d.domain.guid);
        ci_domain.setState_((interfaces::ClientState::Literal)d.domain.state);
        ci_domain.setLast((interfaces::ClientState::Literal)d.domain.last);
        ci_domain.setWatcher(d.domain.watcher);
        ci_domain.setError_(d.domain.error);
        ci_domain.setVersion_(d.domain.version);
        ci_domain.setAttribute_(d.domain.attribute.toJson());
        ci_domain.setMeta(d.domain.meta.toJson());
        ci_domain.setProgress(d.domain.progress);
        ci_domain.setMessage(d.domain.message);
        ci_domain.setAnswer((interfaces::Answer::Literal)d.domain.answer);
        ci_detail.setDomain(std::move(ci_domain));
        const interfaces::Package& ci_package = _getPackage(d.package);
        ci_detail.setPackage_(std::move(ci_package));
        interfaces::Transfers ci_transfers;
        for (const Transfer& transfer : d.transfers) {
            interfaces::Transfer ci_transfer;
            ci_transfer.setDomain(transfer.domain);
            ci_transfer.setName(transfer.name);
            ci_transfer.setProgress(transfer.progress);
            ci_transfer.setSpeed(transfer.speed);
            ci_transfer.setTotal(transfer.total);
            ci_transfer.setCurrent(transfer.current);
            ci_transfer.setPass(transfer.pass);
            ci_transfer.setLeft(transfer.left);
            ci_transfers.push_back(std::move(ci_transfer));
        }
        ci_detail.setTransfers(std::move(ci_transfers));
        ci_detail.setProgress(d.progress);
        ci_detail.setDeploy(d.deploy.get());
        ci_details.push_back(std::move(ci_detail));
    }
    ci_detailMessage.setDetails(std::move(ci_details));
    return ci_detailMessage;
}

static DomainMessage _getDomainMessage(const interfaces::DomainMessage& ci_domainMessage)
{
    Domain domain(ci_domainMessage.getDomain().getName(), ci_domainMessage.getDomain().getGuid());
    domain.state = (ClientState)ci_domainMessage.getDomain().getState_().value_;
    domain.last = (ClientState)ci_domainMessage.getDomain().getLast().value_;
    domain.watcher = ci_domainMessage.getDomain().getWatcher();
    domain.error = ci_domainMessage.getDomain().getError_();
    domain.version = ci_domainMessage.getDomain().getVersion_();
    domain.attribute = Variant::readJson(ci_domainMessage.getDomain().getAttribute_());
    domain.meta = Variant::readJson(ci_domainMessage.getDomain().getMeta());
    domain.progress = ci_domainMessage.getDomain().getProgress();
    domain.message = ci_domainMessage.getDomain().getMessage();
    domain.answer = (Answer)ci_domainMessage.getDomain().getAnswer().value_;
    bool discovery = ci_domainMessage.getDiscovery();
    return DomainMessage(std::move(domain), discovery);
}

class ServerInterfaceAdapter : public ServerInterface, protected interfacesStubDefault {
public:
    ServerInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_server.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        std::shared_ptr<interfacesStubDefault> ptr = std::shared_ptr<interfacesStubDefault>((interfacesStubDefault*)this);
        CommonAPI::Runtime::get()->registerService("local", "commonapi.mifsa.ota.interfaces", ptr, "mifsa_ota_server");
    }
    ~ServerInterfaceAdapter()
    {
        CommonAPI::Runtime::get().reset();
    }
    virtual void sendControlMessage(const ControlMessage& controlMessage) override
    {
        interfacesStubDefault::fireDispatchControlMessageEvent(_getControlMessage(controlMessage));
    }
    virtual void sendDetailMessage(const DetailMessage& detailMessage) override
    {
        interfacesStubDefault::fireDispatchDetailMessageEvent(_getDetailMessage(detailMessage));
    }
    virtual void setCbReportDomain(const CbDomain& cb) override
    {
        m_cbDomain = cb;
    }

protected:
    virtual void invokeDomainMessage(const std::shared_ptr<CommonAPI::ClientId> client, interfaces::DomainMessage ci_domainMessage) override
    {
        MIFSA_UNUSED(client);
        if (m_cbDomain) {
            m_cbDomain(_getDomainMessage(ci_domainMessage));
        }
    }

private:
    CbDomain m_cbDomain;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_SERVER_INTERFACE_VSOMEIP_H
