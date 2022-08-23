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
static mifsa_ota_idl::Package _getPackage(const Package& package)
{
    mifsa_ota_idl::Package t_package;
    t_package.set_domain(package.domain);
    t_package.set_part(package.part);
    t_package.set_version(package.version);
    t_package.set_meta(package.meta.toJson());
    for (const auto& file : package.files) {
        auto* t_fileList = t_package.add_files();
        t_fileList->set_domain(file.domain);
        t_fileList->set_name(file.name);
        t_fileList->set_url(file.url);
        t_fileList->set_size(file.size);
        t_fileList->set_md5(file.md5);
        t_fileList->set_sha1(file.sha1);
        t_fileList->set_sha256(file.sha256);
    }
    return t_package;
}

static mifsa_ota_idl::ControlMessage _getControlMessage(const ControlMessage& controlMessage)
{
    mifsa_ota_idl::ControlMessage t_controlMessage;
    t_controlMessage.set_id(controlMessage.id);
    t_controlMessage.set_control((mifsa_ota_idl::ControlMessage_Control)controlMessage.control);
    t_controlMessage.mutable_upgrade()->set_id(controlMessage.upgrade.id);
    t_controlMessage.mutable_upgrade()->set_download((mifsa_ota_idl::ControlMessage_Upgrade_Method)controlMessage.upgrade.download);
    t_controlMessage.mutable_upgrade()->set_deploy((mifsa_ota_idl::ControlMessage_Upgrade_Method)controlMessage.upgrade.deploy);
    t_controlMessage.mutable_upgrade()->set_maintenance(controlMessage.upgrade.maintenance);
    for (const auto& package : controlMessage.upgrade.packages) {
        auto* t_package = t_controlMessage.mutable_upgrade()->add_packages();
        *t_package = _getPackage(package);
    }
    for (const auto& n : controlMessage.depends) {
        auto* t_n = t_controlMessage.add_depends();
        *t_n = n;
    }
    return t_controlMessage;
}

static mifsa_ota_idl::DetailMessage _getDetailMessage(const DetailMessage& detailMessage)
{
    mifsa_ota_idl::DetailMessage t_detailMessage;
    t_detailMessage.set_id(detailMessage.id);
    t_detailMessage.set_state((mifsa_ota_idl::DetailMessage_ServerState)detailMessage.state);
    t_detailMessage.set_last((mifsa_ota_idl::DetailMessage_ServerState)detailMessage.last);
    t_detailMessage.set_active(detailMessage.active);
    t_detailMessage.set_error(detailMessage.error);
    t_detailMessage.set_step(detailMessage.step);
    t_detailMessage.set_progress(detailMessage.progress);
    t_detailMessage.set_message(detailMessage.message);
    for (const auto& d : detailMessage.details) {
        auto* t_detail = t_detailMessage.add_details();
        t_detail->mutable_domain()->set_name(d.domain.name);
        t_detail->mutable_domain()->set_guid(d.domain.guid);
        t_detail->mutable_domain()->set_state((mifsa_ota_idl::Domain_ClientState)d.domain.state);
        t_detail->mutable_domain()->set_last((mifsa_ota_idl::Domain_ClientState)d.domain.last);
        t_detail->mutable_domain()->set_watcher(d.domain.watcher);
        t_detail->mutable_domain()->set_error(d.domain.error);
        t_detail->mutable_domain()->set_version(d.domain.version);
        t_detail->mutable_domain()->set_attribute(d.domain.attribute.toJson());
        t_detail->mutable_domain()->set_meta(d.domain.meta.toJson());
        t_detail->mutable_domain()->set_progress(d.domain.progress);
        t_detail->mutable_domain()->set_message(d.domain.message);
        t_detail->mutable_domain()->set_answer((mifsa_ota_idl::Domain_Answer)d.domain.answer);
        *(t_detail->mutable_package()) = _getPackage(d.package);
        for (const auto& transfer : d.transfers) {
            auto* t_transfer = t_detail->add_transfers();
            t_transfer->set_domain(transfer.domain);
            t_transfer->set_name(transfer.name);
            t_transfer->set_progress(transfer.progress);
            t_transfer->set_speed(transfer.speed);
            t_transfer->set_total(transfer.total);
            t_transfer->set_current(transfer.current);
            t_transfer->set_pass(transfer.pass);
            t_transfer->set_left(transfer.left);
        }
        t_detail->set_progress(d.progress);
        t_detail->set_deploy(d.deploy.get());
    }
    return t_detailMessage;
}

static DomainMessage _getDomainMessage(const mifsa_ota_idl::DomainMessage& t_domainMessage)
{
    DomainMessage domainMessage;
    domainMessage.domain.name = t_domainMessage.domain().name();
    domainMessage.domain.guid = t_domainMessage.domain().guid();
    domainMessage.domain.state = (ClientState)t_domainMessage.domain().state();
    domainMessage.domain.last = (ClientState)t_domainMessage.domain().last();
    domainMessage.domain.watcher = t_domainMessage.domain().watcher();
    domainMessage.domain.error = t_domainMessage.domain().error();
    domainMessage.domain.version = t_domainMessage.domain().version();
    domainMessage.domain.attribute = Variant::readJson(t_domainMessage.domain().attribute());
    domainMessage.domain.meta = Variant::readJson(t_domainMessage.domain().meta());
    domainMessage.domain.progress = t_domainMessage.domain().progress();
    domainMessage.domain.message = t_domainMessage.domain().message();
    domainMessage.domain.answer = (Answer)t_domainMessage.domain().answer();
    domainMessage.discovery = t_domainMessage.discovery();
    return domainMessage;
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
    virtual void onStarted() override
    {
    }
    virtual void onStoped() override
    {
    }
    void sendControlMessage(const ControlMessage& controlMessage) override
    {
        const auto& t_controlMessage = _getControlMessage(controlMessage);
        CFdbProtoMsgBuilder builder(t_controlMessage);
        CBaseServer::broadcast(mifsa_ota_idl::TP_CONTROL_MSG, builder);
    }
    void sendDetailMessage(const DetailMessage& detailMessage) override
    {
        const auto& t_detailMessage = _getDetailMessage(detailMessage);
        CFdbProtoMsgBuilder builder(t_detailMessage);
        CBaseServer::broadcast(mifsa_ota_idl::TP_DETAIL_MSG, builder);
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
        if (msgData->code() == mifsa_ota_idl::TP_DOMAIN_MSG) {
            mifsa_ota_idl::DomainMessage t_domainMessage;
            if (msgData->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(t_domainMessage);
                if (!msgData->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            const auto& domainMessage = _getDomainMessage(t_domainMessage);
            if (m_cbDomain) {
                m_cbDomain(domainMessage);
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
