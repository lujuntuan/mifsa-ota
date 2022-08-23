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

#include "server.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/mifsa_ota_idl/CommonStubDefault.hpp>

using namespace v1_0;

MIFSA_NAMESPACE_BEGIN
namespace Ota {
static mifsa_ota_idl::Common::Package _getPackage(const Package& package)
{
    mifsa_ota_idl::Common::Package t_package;
    t_package.setDomain(package.domain);
    t_package.setPart(package.part);
    t_package.setVersion_(package.version);
    t_package.setMeta(package.meta.toJson());
    mifsa_ota_idl::Common::Files t_files;
    for (const auto& file : package.files) {
        mifsa_ota_idl::Common::File t_file;
        t_file.setDomain(file.domain);
        t_file.setName(file.name);
        t_file.setUrl(file.url);
        t_file.setSize(file.size);
        t_file.setMd5(file.md5);
        t_file.setSha1(file.sha1);
        t_file.setSha256(file.sha256);
        t_files.push_back(std::move(t_file));
    }
    t_package.setFiles(t_files);
    return t_package;
}

static mifsa_ota_idl::Common::ControlMessage _getControlMessage(const ControlMessage& controlMessage)
{
    mifsa_ota_idl::Common::ControlMessage t_controlMessage;
    t_controlMessage.setId(controlMessage.id);
    t_controlMessage.setControl((mifsa_ota_idl::Common::Control::Literal)controlMessage.control);
    mifsa_ota_idl::Common::Upgrade t_upgrade;
    t_upgrade.setId(controlMessage.upgrade.id);
    t_upgrade.setDownload((mifsa_ota_idl::Common::Method::Literal)controlMessage.upgrade.download);
    t_upgrade.setDeploy((mifsa_ota_idl::Common::Method::Literal)controlMessage.upgrade.deploy);
    t_upgrade.setMaintenance(controlMessage.upgrade.maintenance);
    mifsa_ota_idl::Common::Packages t_packages;
    for (const auto& package : controlMessage.upgrade.packages) {
        const mifsa_ota_idl::Common::Package& t_package = _getPackage(package);
        t_packages.push_back(std::move(t_package));
    }
    t_upgrade.setPackages(std::move(t_packages));
    t_controlMessage.setUpgrade(std::move(t_upgrade));
    mifsa_ota_idl::Common::Depends t_depends;
    for (const auto& str : controlMessage.depends) {
        mifsa_ota_idl::Common::Depend t_depend;
        t_depend.setData(str);
        t_depends.push_back(std::move(t_depend));
    }
    t_controlMessage.setDepends(t_depends);
    return t_controlMessage;
}

static mifsa_ota_idl::Common::DetailMessage _getDetailMessage(const DetailMessage& detailMessage)
{
    mifsa_ota_idl::Common::DetailMessage t_detailMessage;
    t_detailMessage.setId(detailMessage.id);
    t_detailMessage.setState_((mifsa_ota_idl::Common::ServerState::Literal)detailMessage.state);
    t_detailMessage.setLast((mifsa_ota_idl::Common::ServerState::Literal)detailMessage.last);
    t_detailMessage.setActive(detailMessage.active);
    t_detailMessage.setError_(detailMessage.error);
    t_detailMessage.setStep(detailMessage.step);
    t_detailMessage.setProgress(detailMessage.progress);
    t_detailMessage.setMessage(detailMessage.message);
    mifsa_ota_idl::Common::Details t_details;
    for (const auto& d : detailMessage.details) {
        mifsa_ota_idl::Common::Detail t_detail;
        mifsa_ota_idl::Common::Domain t_domain;
        t_domain.setName(d.domain.name);
        t_domain.setGuid(d.domain.guid);
        t_domain.setState_((mifsa_ota_idl::Common::ClientState::Literal)d.domain.state);
        t_domain.setLast((mifsa_ota_idl::Common::ClientState::Literal)d.domain.last);
        t_domain.setWatcher(d.domain.watcher);
        t_domain.setError_(d.domain.error);
        t_domain.setVersion_(d.domain.version);
        t_domain.setAttribute_(d.domain.attribute.toJson());
        t_domain.setMeta(d.domain.meta.toJson());
        t_domain.setProgress(d.domain.progress);
        t_domain.setMessage(d.domain.message);
        t_domain.setAnswer((mifsa_ota_idl::Common::Answer::Literal)d.domain.answer);
        t_detail.setDomain(std::move(t_domain));
        const mifsa_ota_idl::Common::Package& t_package = _getPackage(d.package);
        t_detail.setPackage_(std::move(t_package));
        mifsa_ota_idl::Common::Transfers t_transfers;
        for (const auto& transfer : d.transfers) {
            mifsa_ota_idl::Common::Transfer t_transfer;
            t_transfer.setDomain(transfer.domain);
            t_transfer.setName(transfer.name);
            t_transfer.setProgress(transfer.progress);
            t_transfer.setSpeed(transfer.speed);
            t_transfer.setTotal(transfer.total);
            t_transfer.setCurrent(transfer.current);
            t_transfer.setPass(transfer.pass);
            t_transfer.setLeft(transfer.left);
            t_transfers.push_back(std::move(t_transfer));
        }
        t_detail.setTransfers(std::move(t_transfers));
        t_detail.setProgress(d.progress);
        t_detail.setDeploy(d.deploy.get());
        t_details.push_back(std::move(t_detail));
    }
    t_detailMessage.setDetails(std::move(t_details));
    return t_detailMessage;
}

static DomainMessage _getDomainMessage(const mifsa_ota_idl::Common::DomainMessage& t_domainMessage)
{
    DomainMessage domainMessage;
    domainMessage.domain.name = t_domainMessage.getDomain().getName();
    domainMessage.domain.guid = t_domainMessage.getDomain().getGuid();
    domainMessage.domain.state = (ClientState)t_domainMessage.getDomain().getState_().value_;
    domainMessage.domain.last = (ClientState)t_domainMessage.getDomain().getLast().value_;
    domainMessage.domain.watcher = t_domainMessage.getDomain().getWatcher();
    domainMessage.domain.error = t_domainMessage.getDomain().getError_();
    domainMessage.domain.version = t_domainMessage.getDomain().getVersion_();
    domainMessage.domain.attribute = Variant::readJson(t_domainMessage.getDomain().getAttribute_());
    domainMessage.domain.meta = Variant::readJson(t_domainMessage.getDomain().getMeta());
    domainMessage.domain.progress = t_domainMessage.getDomain().getProgress();
    domainMessage.domain.message = t_domainMessage.getDomain().getMessage();
    domainMessage.domain.answer = (Answer)t_domainMessage.getDomain().getAnswer().value_;
    domainMessage.discovery = t_domainMessage.getDiscovery();
    return domainMessage;
}

class ServerInterfaceAdapter : public ServerInterface, protected mifsa_ota_idl::CommonStubDefault {
public:
    ServerInterfaceAdapter()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_server.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        std::shared_ptr<mifsa_ota_idl::CommonStubDefault> ptr = std::shared_ptr<mifsa_ota_idl::CommonStubDefault>((mifsa_ota_idl::CommonStubDefault*)this);
        CommonAPI::Runtime::get()->registerService("local", "mifsa_ota_idl.Common", ptr, "mifsa_ota_server");
    }
    ~ServerInterfaceAdapter()
    {
        CommonAPI::Runtime::get().reset();
    }
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    virtual void sendControlMessage(const ControlMessage& controlMessage) override
    {
        const auto& t_controlMessage = _getControlMessage(controlMessage);
        mifsa_ota_idl::CommonStubDefault::fireDispatchControlMessageEvent(t_controlMessage);
    }
    virtual void sendDetailMessage(const DetailMessage& detailMessage) override
    {
        const auto& t_detailMessage = _getDetailMessage(detailMessage);
        mifsa_ota_idl::CommonStubDefault::fireDispatchDetailMessageEvent(t_detailMessage);
    }
    virtual void setCbReportDomain(const CbDomain& cb) override
    {
        m_cbDomain = cb;
    }

protected:
    virtual void invokeDomainMessage(const std::shared_ptr<CommonAPI::ClientId> client, mifsa_ota_idl::Common::DomainMessage t_domainMessage) override
    {
        MIFSA_UNUSED(client);
        const auto& domainMessage = _getDomainMessage(t_domainMessage);
        if (m_cbDomain) {
            m_cbDomain(domainMessage);
        }
    }

private:
    CbDomain m_cbDomain;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_SERVER_INTERFACE_VSOMEIP_H
