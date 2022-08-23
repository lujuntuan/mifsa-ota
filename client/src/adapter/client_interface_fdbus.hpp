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

#ifndef MIFSA_OTA_CLIENT_INTERFACE_FDBUS_H
#define MIFSA_OTA_CLIENT_INTERFACE_FDBUS_H

#ifdef WIN32
#ifndef __WIN32__
#define __WIN32__
#endif
#endif

#ifdef FDBUS_NEW_INTERFACE
#define FDBUS_ONLINE_ARG_TYPE const CFdbOnlineInfo&
#include <fdbus/CFdbProtoMsgBuilder.h>
#include <fdbus/fdbus.h>
using namespace ipc::fdbus;
#else
#define FDBUS_ONLINE_ARG_TYPE FdbSessionId_t, bool
#include <common_base/CFdbProtoMsgBuilder.h>
#include <common_base/fdbus.h>
#endif

#include "mifsa/ota/client.h"
#include "mifsa/ota/idl/fdbus/ota.pb.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
static Package _getPackage(const mifsa_ota_idl::Package& t_package)
{
    Package package;
    package.domain = t_package.domain();
    package.part = t_package.part();
    package.version = t_package.version();
    package.meta = Variant::readJson(t_package.meta());
    for (int i = 0; i < t_package.files_size(); i++) {
        File file;
        const auto& t_file = t_package.files(i);
        file.domain = t_file.domain();
        file.name = t_file.name();
        file.url = t_file.url();
        file.size = t_file.size();
        file.md5 = t_file.md5();
        file.sha1 = t_file.sha1();
        file.sha256 = t_file.sha256();
        package.files.push_back(std::move(file));
    }
    return package;
}

static ControlMessage _getControlMessage(const mifsa_ota_idl::ControlMessage& t_controlMessage)
{
    ControlMessage controlMessage;
    controlMessage.id = t_controlMessage.id();
    controlMessage.control = (Control)t_controlMessage.control();
    controlMessage.upgrade.id = t_controlMessage.upgrade().id();
    controlMessage.upgrade.download = (Upgrade::Method)t_controlMessage.upgrade().download();
    controlMessage.upgrade.deploy = (Upgrade::Method)t_controlMessage.upgrade().deploy();
    controlMessage.upgrade.maintenance = t_controlMessage.upgrade().maintenance();
    for (int i = 0; i < t_controlMessage.upgrade().packages_size(); i++) {
        const auto& t_package = t_controlMessage.upgrade().packages(i);
        const Package& package = _getPackage(t_package);
        controlMessage.upgrade.packages.push_back(std::move(package));
    }
    for (int i = 0; i < t_controlMessage.depends_size(); i++) {
        const auto& t_name = t_controlMessage.depends(i);
        controlMessage.depends.push_back(std::move(t_name));
    }
    return controlMessage;
}

static DetailMessage _getDetailMessage(const mifsa_ota_idl::DetailMessage& t_detailMessage)
{
    DetailMessage detailMessage;
    detailMessage.id = t_detailMessage.id();
    detailMessage.state = (ServerState)t_detailMessage.state();
    detailMessage.last = (ServerState)t_detailMessage.last();
    detailMessage.active = t_detailMessage.active();
    detailMessage.error = t_detailMessage.error();
    detailMessage.step = t_detailMessage.step();
    detailMessage.progress = t_detailMessage.progress();
    detailMessage.message = t_detailMessage.message();
    for (int i = 0; i < t_detailMessage.details_size(); i++) {
        const auto& t_detail = t_detailMessage.details(i);
        Domain domain(t_detail.domain().name(), t_detail.domain().guid());
        domain.state = (ClientState)t_detail.domain().state();
        domain.last = (ClientState)t_detail.domain().last();
        domain.watcher = t_detail.domain().watcher();
        domain.error = t_detail.domain().error();
        domain.version = t_detail.domain().version();
        domain.attribute = Variant::readJson(t_detail.domain().attribute());
        domain.meta = Variant::readJson(t_detail.domain().meta());
        domain.progress = t_detail.domain().progress();
        domain.message = t_detail.domain().message();
        domain.answer = (Answer)t_detail.domain().answer();
        Detail detail(std::move(domain));
        const Package& package = _getPackage(t_detail.package());
        detail.package = package;
        for (int j = 0; j < t_detail.transfers_size(); j++) {
            const auto& t_transfer = t_detail.transfers(j);
            Transfer transfer;
            transfer.domain = t_transfer.domain();
            transfer.name = t_transfer.name();
            transfer.progress = t_transfer.progress();
            transfer.speed = t_transfer.speed();
            transfer.total = t_transfer.total();
            transfer.current = t_transfer.current();
            transfer.pass = t_transfer.pass();
            transfer.left = t_transfer.left();
            detail.transfers.push_back(std::move(transfer));
        }
        detail.progress = t_detail.progress();
        if (t_detail.deploy() > 0) {
            detail.deploy.start(Elapsed::current() - t_detail.deploy());
        }
        detailMessage.details.push_back(std::move(detail));
    }
    return detailMessage;
}

static mifsa_ota_idl::DomainMessage _getDomainMessage(const DomainMessage& domainMessage)
{
    mifsa_ota_idl::DomainMessage t_domainMessage;
    t_domainMessage.mutable_domain()->set_name(domainMessage.domain.name);
    t_domainMessage.mutable_domain()->set_guid(domainMessage.domain.guid);
    t_domainMessage.mutable_domain()->set_state((mifsa_ota_idl::Domain_ClientState)domainMessage.domain.state);
    t_domainMessage.mutable_domain()->set_last((mifsa_ota_idl::Domain_ClientState)domainMessage.domain.last);
    t_domainMessage.mutable_domain()->set_watcher(domainMessage.domain.watcher);
    t_domainMessage.mutable_domain()->set_error(domainMessage.domain.error);
    t_domainMessage.mutable_domain()->set_version(domainMessage.domain.version);
    t_domainMessage.mutable_domain()->set_attribute(domainMessage.domain.attribute.toJson());
    t_domainMessage.mutable_domain()->set_meta(domainMessage.domain.meta.toJson());
    t_domainMessage.mutable_domain()->set_progress(domainMessage.domain.progress);
    t_domainMessage.mutable_domain()->set_message(domainMessage.domain.message);
    t_domainMessage.mutable_domain()->set_answer((mifsa_ota_idl::Domain_Answer)domainMessage.domain.answer);
    t_domainMessage.set_discovery(domainMessage.discovery);
    return t_domainMessage;
}

class ClientInterfaceAdapter : public ClientInterface, protected CBaseClient {
public:
    ClientInterfaceAdapter()
        : CBaseClient("mifsa_ota_client", &m_worker)
    {
        FDB_CONTEXT->start();
        m_worker.start();
        CBaseClient::connect("svc://mifsa_ota");
    }
    ~ClientInterfaceAdapter()
    {
        m_worker.flush();
        m_worker.exit();
        CBaseClient::disconnect();
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
        return CBaseClient::connected();
    }
    virtual void setCbControlMessage(CbControlMessage cb) override
    {
        m_cbControlMessage = cb;
    }
    virtual void setCbDetailMessage(CbDetailMessage cb) override
    {
        m_cbDetailMessage = cb;
        CFdbMsgSubscribeList subList;
        if (mifsa_ota_client->hasSubscibeDetail()) {
            CBaseClient::addNotifyItem(subList, mifsa_ota_idl::TP_DETAIL_MSG);
        }
        CBaseClient::subscribe(subList);
    }
    virtual bool sendDomain(const DomainMessage& domainMessage) override
    {
        if (!ClientInterfaceAdapter::connected()) {
            return false;
        }
        mifsa_ota_idl::DomainMessage t_domainMessage = _getDomainMessage(domainMessage);
        CFdbProtoMsgBuilder builder(t_domainMessage);
        CBaseClient::invoke(mifsa_ota_idl::TP_DOMAIN_MSG, builder);
        return true;
    }

protected:
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        CFdbMsgSubscribeList subList;
        CBaseClient::addNotifyItem(subList, mifsa_ota_idl::TP_CONTROL_MSG);
        if (mifsa_ota_client->hasSubscibeDetail()) {
            CBaseClient::addNotifyItem(subList, mifsa_ota_idl::TP_DETAIL_MSG);
        }
        CBaseClient::subscribe(subList);
        cbConnected(true);
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
        cbConnected(false);
    }
    void onBroadcast(CBaseJob::Ptr& msg_ref) override
    {
        CFdbMessage* msgData = castToMessage<CBaseMessage*>(msg_ref);
        if (!msgData) {
            return;
        }
        if (msgData->code() == mifsa_ota_idl::TP_CONTROL_MSG) {
            mifsa_ota_idl::ControlMessage t_controlMessage;
            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(t_controlMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            if (checkControlMessageId && !checkControlMessageId(t_controlMessage.id())) {
                return;
            }
            const auto& controlMessage = _getControlMessage(t_controlMessage);
            if (m_cbControlMessage) {
                m_cbControlMessage(controlMessage);
            }
        } else if (msgData->code() == mifsa_ota_idl::TP_DETAIL_MSG) {
            mifsa_ota_idl::DetailMessage t_detailMessage;

            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(t_detailMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            if (checkDetailMessageId && !checkDetailMessageId(t_detailMessage.id())) {
                return;
            }
            const auto& detailMessage = _getDetailMessage(t_detailMessage);
            if (m_cbDetailMessage) {
                m_cbDetailMessage(detailMessage);
            }
        }
    }

private:
    CBaseWorker m_worker;
    CbControlMessage m_cbControlMessage;
    CbDetailMessage m_cbDetailMessage;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CLIENT_INTERFACE_FDBUS_H
