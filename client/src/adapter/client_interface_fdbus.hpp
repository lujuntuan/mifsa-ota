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

int _time_out = 5000;

namespace Ota {
static Package _getPackage(const mifsa::ota::pb::Package& pb_package)
{
    Package package;
    package.domain = pb_package.domain();
    package.part = pb_package.part();
    package.version = pb_package.version();
    package.meta = Variant::readJson(pb_package.meta());
    for (int i = 0; i < pb_package.files_size(); i++) {
        File file;
        const auto& pb_file = pb_package.files(i);
        file.domain = pb_file.domain();
        file.name = pb_file.name();
        file.url = pb_file.url();
        file.size = pb_file.size();
        file.md5 = pb_file.md5();
        file.sha1 = pb_file.sha1();
        file.sha256 = pb_file.sha256();
        package.files.push_back(std::move(file));
    }
    return package;
}

static ControlMessage _getControlMessage(const mifsa::ota::pb::ControlMessage& pb_controlMessage)
{
    uint32_t id = pb_controlMessage.id();
    Control control = (Control)pb_controlMessage.control();
    Upgrade upgrade;
    upgrade.id = pb_controlMessage.upgrade().id();
    upgrade.download = (Upgrade::Method)pb_controlMessage.upgrade().download();
    upgrade.deploy = (Upgrade::Method)pb_controlMessage.upgrade().deploy();
    upgrade.maintenance = pb_controlMessage.upgrade().maintenance();
    for (int i = 0; i < pb_controlMessage.upgrade().packages_size(); i++) {
        const auto& pb_package = pb_controlMessage.upgrade().packages(i);
        const Package& package = _getPackage(pb_package);
        upgrade.packages.push_back(std::move(package));
    }
    Depends depends;
    for (int i = 0; i < pb_controlMessage.depends_size(); i++) {
        const auto& pb_name = pb_controlMessage.depends(i);
        depends.push_back(std::move(pb_name));
    }
    return ControlMessage(id, control, std::move(upgrade), std::move(depends));
}

static DetailMessage _getDetailMessage(const mifsa::ota::pb::DetailMessage& pb_detailMessage)
{
    uint32_t id = pb_detailMessage.id();
    ServerState state = (ServerState)pb_detailMessage.state();
    ServerState last = (ServerState)pb_detailMessage.last();
    bool active = pb_detailMessage.active();
    int error = pb_detailMessage.error();
    float step = pb_detailMessage.step();
    float progress = pb_detailMessage.progress();
    const std::string& message = pb_detailMessage.message();
    Details details;
    for (int i = 0; i < pb_detailMessage.details_size(); i++) {
        const auto& pb_detail = pb_detailMessage.details(i);
        Domain domain(pb_detail.domain().name(), pb_detail.domain().guid());
        domain.state = (ClientState)pb_detail.domain().state();
        domain.last = (ClientState)pb_detail.domain().last();
        domain.watcher = pb_detail.domain().watcher();
        domain.error = pb_detail.domain().error();
        domain.version = pb_detail.domain().version();
        domain.attribute = Variant::readJson(pb_detail.domain().attribute());
        domain.meta = Variant::readJson(pb_detail.domain().meta());
        domain.progress = pb_detail.domain().progress();
        domain.message = pb_detail.domain().message();
        domain.answer = (Answer)pb_detail.domain().answer();
        Detail detail(std::move(domain));
        const Package& package = _getPackage(pb_detail.package());
        detail.package = package;
        for (int j = 0; j < pb_detail.transfers_size(); j++) {
            const auto& pb_transfer = pb_detail.transfers(j);
            Transfer transfer;
            transfer.domain = pb_transfer.domain();
            transfer.name = pb_transfer.name();
            transfer.progress = pb_transfer.progress();
            transfer.speed = pb_transfer.speed();
            transfer.total = pb_transfer.total();
            transfer.current = pb_transfer.current();
            transfer.pass = pb_transfer.pass();
            transfer.left = pb_transfer.left();
            detail.transfers.push_back(std::move(transfer));
        }
        detail.progress = pb_detail.progress();
        if (pb_detail.deploy() > 0) {
            detail.deploy.start(Elapsed::current() - pb_detail.deploy());
        }
        details.push_back(std::move(detail));
    }
    return DetailMessage(id, state, last, active, error, step, progress, std::move(message), std::move(details));
}

static mifsa::ota::pb::DomainMessage _getDomainMessage(const DomainMessage& domainMessage)
{
    mifsa::ota::pb::DomainMessage pb_domainMessage;
    pb_domainMessage.mutable_domain()->set_name(domainMessage.domain.name);
    pb_domainMessage.mutable_domain()->set_guid(domainMessage.domain.guid);
    pb_domainMessage.mutable_domain()->set_state((mifsa::ota::pb::Domain_ClientState)domainMessage.domain.state);
    pb_domainMessage.mutable_domain()->set_last((mifsa::ota::pb::Domain_ClientState)domainMessage.domain.last);
    pb_domainMessage.mutable_domain()->set_watcher(domainMessage.domain.watcher);
    pb_domainMessage.mutable_domain()->set_error(domainMessage.domain.error);
    pb_domainMessage.mutable_domain()->set_version(domainMessage.domain.version);
    pb_domainMessage.mutable_domain()->set_attribute(domainMessage.domain.attribute.toJson());
    pb_domainMessage.mutable_domain()->set_meta(domainMessage.domain.meta.toJson());
    pb_domainMessage.mutable_domain()->set_progress(domainMessage.domain.progress);
    pb_domainMessage.mutable_domain()->set_message(domainMessage.domain.message);
    pb_domainMessage.mutable_domain()->set_answer((mifsa::ota::pb::Domain_Answer)domainMessage.domain.answer);
    pb_domainMessage.set_discovery(domainMessage.discovery);
    return pb_domainMessage;
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
    }
    virtual bool sendDomain(const DomainMessage& domainMessage) override
    {
        if (!CBaseClient::connected()) {
            return false;
        }
        mifsa::ota::pb::DomainMessage pb_domainMessage = _getDomainMessage(domainMessage);
        CFdbProtoMsgBuilder builder(pb_domainMessage);
        CBaseClient::invoke(mifsa::ota::pb::TP_DOMAIN_MSG, builder);
        return true;
    }

protected:
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        CFdbMsgSubscribeList subList;
        CBaseClient::addNotifyItem(subList, mifsa::ota::pb::TP_CONTROL_MSG);
        if (mifsa_ota_client->hasSubscibeDetail()) {
            CBaseClient::addNotifyItem(subList, mifsa::ota::pb::TP_DETAIL_MSG);
        }
        CBaseClient::subscribe(subList);
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
        if (msgData->code() == mifsa::ota::pb::TP_CONTROL_MSG) {
            mifsa::ota::pb::ControlMessage pb_controlMessage;
            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_controlMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            mifsa_ota_client->processControlMessage(_getControlMessage(pb_controlMessage));
        } else if (msgData->code() == mifsa::ota::pb::TP_DETAIL_MSG) {
            mifsa::ota::pb::DetailMessage pb_detailMessage;
            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_detailMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            mifsa_ota_client->processDetailMessage(_getDetailMessage(pb_detailMessage));
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
