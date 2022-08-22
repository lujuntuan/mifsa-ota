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

#ifndef MIFSA_OTA_SERVER_INTERFACE_FDBUS_H
#define MIFSA_OTA_SERVER_INTERFACE_FDBUS_H

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

#include "mifsa/ota/idl/fdbus/ota.pb.h"
#include "server.h"
#include <mifsa/base/thread.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
static mifsa::ota::pb::Package _getPackage(const Package& package)
{
    mifsa::ota::pb::Package pb_package;
    pb_package.set_domain(package.domain);
    pb_package.set_part(package.part);
    pb_package.set_version(package.version);
    pb_package.set_meta(package.meta.toJson());
    for (const File& file : package.files) {
        auto* pb_fileList = pb_package.add_files();
        pb_fileList->set_domain(file.domain);
        pb_fileList->set_name(file.name);
        pb_fileList->set_url(file.url);
        pb_fileList->set_size(file.size);
        pb_fileList->set_md5(file.md5);
        pb_fileList->set_sha1(file.sha1);
        pb_fileList->set_sha256(file.sha256);
    }
    return pb_package;
}

static mifsa::ota::pb::ControlMessage _getControlMessage(const ControlMessage& controlMessage)
{
    mifsa::ota::pb::ControlMessage pb_controlMessage;
    pb_controlMessage.set_id(controlMessage.id);
    pb_controlMessage.set_control((mifsa::ota::pb::ControlMessage_Control)controlMessage.control);
    pb_controlMessage.mutable_upgrade()->set_id(controlMessage.upgrade.id);
    pb_controlMessage.mutable_upgrade()->set_download((mifsa::ota::pb::ControlMessage_Upgrade_Method)controlMessage.upgrade.download);
    pb_controlMessage.mutable_upgrade()->set_deploy((mifsa::ota::pb::ControlMessage_Upgrade_Method)controlMessage.upgrade.deploy);
    pb_controlMessage.mutable_upgrade()->set_maintenance(controlMessage.upgrade.maintenance);
    for (const Package& package : controlMessage.upgrade.packages) {
        auto* pb_package = pb_controlMessage.mutable_upgrade()->add_packages();
        *pb_package = _getPackage(package);
    }
    for (const std::string& n : controlMessage.depends) {
        auto* pb_n = pb_controlMessage.add_depends();
        *pb_n = n;
    }
    return pb_controlMessage;
}

static mifsa::ota::pb::DetailMessage _getDetailMessage(const DetailMessage& detailMessage)
{
    mifsa::ota::pb::DetailMessage pb_detailMessage;
    pb_detailMessage.set_id(detailMessage.id);
    pb_detailMessage.set_state((mifsa::ota::pb::DetailMessage_ServerState)detailMessage.state);
    pb_detailMessage.set_last((mifsa::ota::pb::DetailMessage_ServerState)detailMessage.last);
    pb_detailMessage.set_active(detailMessage.active);
    pb_detailMessage.set_error(detailMessage.error);
    pb_detailMessage.set_step(detailMessage.step);
    pb_detailMessage.set_progress(detailMessage.progress);
    pb_detailMessage.set_message(detailMessage.message);
    for (const auto& d : detailMessage.details) {
        auto* pb_detail = pb_detailMessage.add_details();
        pb_detail->mutable_domain()->set_name(d.domain.name);
        pb_detail->mutable_domain()->set_guid(d.domain.guid);
        pb_detail->mutable_domain()->set_state((mifsa::ota::pb::Domain_ClientState)d.domain.state);
        pb_detail->mutable_domain()->set_last((mifsa::ota::pb::Domain_ClientState)d.domain.last);
        pb_detail->mutable_domain()->set_watcher(d.domain.watcher);
        pb_detail->mutable_domain()->set_error(d.domain.error);
        pb_detail->mutable_domain()->set_version(d.domain.version);
        pb_detail->mutable_domain()->set_attribute(d.domain.attribute.toJson());
        pb_detail->mutable_domain()->set_meta(d.domain.meta.toJson());
        pb_detail->mutable_domain()->set_progress(d.domain.progress);
        pb_detail->mutable_domain()->set_message(d.domain.message);
        pb_detail->mutable_domain()->set_answer((mifsa::ota::pb::Domain_Answer)d.domain.answer);
        *(pb_detail->mutable_package()) = _getPackage(d.package);
        for (const Transfer& transfer : d.transfers) {
            auto* pb_transfer = pb_detail->add_transfers();
            pb_transfer->set_domain(transfer.domain);
            pb_transfer->set_name(transfer.name);
            pb_transfer->set_progress(transfer.progress);
            pb_transfer->set_speed(transfer.speed);
            pb_transfer->set_total(transfer.total);
            pb_transfer->set_current(transfer.current);
            pb_transfer->set_pass(transfer.pass);
            pb_transfer->set_left(transfer.left);
        }
        pb_detail->set_progress(d.progress);
        pb_detail->set_deploy(d.deploy.get());
    }
    return pb_detailMessage;
}

static DomainMessage _getDomainMessage(const mifsa::ota::pb::DomainMessage& pb_domainMessage)
{
    Domain domain(pb_domainMessage.domain().name(), pb_domainMessage.domain().guid());
    domain.state = (ClientState)pb_domainMessage.domain().state();
    domain.last = (ClientState)pb_domainMessage.domain().last();
    domain.watcher = pb_domainMessage.domain().watcher();
    domain.error = pb_domainMessage.domain().error();
    domain.version = pb_domainMessage.domain().version();
    domain.attribute = Variant::readJson(pb_domainMessage.domain().attribute());
    domain.meta = Variant::readJson(pb_domainMessage.domain().meta());
    domain.progress = pb_domainMessage.domain().progress();
    domain.message = pb_domainMessage.domain().message();
    domain.answer = (Answer)pb_domainMessage.domain().answer();
    bool discovery = pb_domainMessage.discovery();
    return DomainMessage(std::move(domain), discovery);
}

class ServerInterfaceAdapter : public ServerInterface, protected CBaseServer {
public:
    ServerInterfaceAdapter()
        : CBaseServer("mifsa_ota_client", &m_worker)
    {
        FDB_CONTEXT->start();
        m_worker.start();
        CBaseServer::bind("svc://mifsa_ota");
    }
    ~ServerInterfaceAdapter()
    {
        m_worker.flush();
        m_worker.exit();
        CBaseServer::unbind();
        CBaseServer::disconnect();
    }
    void sendControlMessage(const ControlMessage& controlMessage) override
    {
        const auto& pb_controlMessage = _getControlMessage(controlMessage);
        CFdbProtoMsgBuilder builder(pb_controlMessage);
        CBaseServer::broadcast(mifsa::ota::pb::TP_CONTROL_MSG, builder);
    }
    void sendDetailMessage(const DetailMessage& detailMessage) override
    {
        const auto& pb_detailMessage = _getDetailMessage(detailMessage);
        CFdbProtoMsgBuilder builder(pb_detailMessage);
        CBaseServer::broadcast(mifsa::ota::pb::TP_DETAIL_MSG, builder);
    }
    void setCbReportDomain(const CbDomain& cb) override
    {
        m_cbDomain = cb;
    }

protected:
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
    }
    void onSubscribe(CBaseJob::Ptr& msg_ref) override
    {
    }
    void onInvoke(CBaseJob::Ptr& msg_ref) override
    {
        CFdbMessage* msgData = castToMessage<CBaseMessage*>(msg_ref);
        if (msgData->code() == mifsa::ota::pb::TP_DOMAIN_MSG) {
            mifsa::ota::pb::DomainMessage pb_domainMessage;
            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_domainMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            if (m_cbDomain) {
                m_cbDomain(_getDomainMessage(pb_domainMessage));
            }
        }
    }

private:
    CBaseWorker m_worker;
    CbDomain m_cbDomain;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_SERVER_INTERFACE_FDBUS_H
