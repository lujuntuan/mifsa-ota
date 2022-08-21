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

#ifndef MIFSA_OTA_INTERFACE_REALIZE_FDBUS_H
#define MIFSA_OTA_INTERFACE_REALIZE_FDBUS_H

#ifdef MIFSA_SUPPORT_FDBUS

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
#include "mifsa/ota/interface.h"
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
    return pb_domainMessage;
}

class FdbusClient : public CBaseClient {
public:
    FdbusClient()
        : CBaseClient("mifsa_ota_client", &worker)
    {
        worker.start();
    }
    ~FdbusClient()
    {
        worker.flush();
        worker.exit();
        disconnect();
    }
    void onOnline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOnline!");
        CFdbMsgSubscribeList subList;
        addNotifyItem(subList, mifsa::ota::pb::TP_CONTROL_MSG);
        if (mifsa_ota_client->hasSubscibeDetail()) {
            addNotifyItem(subList, mifsa::ota::pb::TP_DETAIL_MSG);
        }
        subscribe(subList);
        m_sid = sid;
    }
    void onOffline(FDBUS_ONLINE_ARG_TYPE) override
    {
        LOG_DEBUG("onOffline!");
        cbConnected(false);
    }
    void onBroadcast(CBaseJob::Ptr& msg_ref) override
    {
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        if (msg->code() == mifsa::ota::pb::MSG_LOCATION) {
            if (!cbLocation) {
                return;
            }
            mifsa::ota::pb::Location pb_location;
            if (msg->getPayloadSize() > 0) {
                CFdbProtoMsgParser parser(pb_location);
                if (!msg->deserialize(parser)) {
                    LOG_WARNING("deserialize msg error");
                    return;
                }
            }
            Location location;
            location.size = (int)pb_location.size();
            location.flags = pb_location.flags();
            location.latitude = pb_location.latitude();
            location.longitude = pb_location.longitude();
            location.altitude = pb_location.altitude();
            location.speed = pb_location.speed();
            location.bearing = pb_location.bearing();
            location.accuracy = pb_location.accuracy();
            location.timestamp = pb_location.timestamp();
            location.data = pb_location.data();
            cbLocation(location);
        }
    }

public:
    CBaseWorker worker;
    CbConnected cbConnected;
    CbLocation cbLocation;
};

class InterfaceImplementation : public Interface {
public:
    InterfaceImplementation()
    {
        FDB_CONTEXT->start();
        m_client = std::make_unique<FdbusClient>();
        m_client->cbConnected = _cbConnected;
        m_client->connect("svc://mifsa_ota");
    }
    virtual std::string version() override
    {
        return MIFSA_OTA_VERSION;
    }
    virtual bool connected() override
    {
        return m_client->connected();
    }
    virtual std::string getNmea() override
    {
        std::string nmea;
        mifsa::ota::pb::Command pb_command;
        pb_command.set_type(mifsa::ota::pb::Command_Type_QUERY_NMEA);
        CFdbProtoMsgBuilder builder(pb_command);
        CBaseJob::Ptr msg_ref(new CBaseMessage(mifsa::ota::pb::MSG_COMMAND));
        m_client->invoke(msg_ref, builder, _time_out);
        auto msg = castToMessage<CBaseMessage*>(msg_ref);
        mifsa::ota::pb::Nmea pb_nmea;
        if (msg->getPayloadSize() > 0) {
            CFdbProtoMsgParser parser(pb_nmea);
            if (!msg->deserialize(parser)) {
                LOG_WARNING("deserialize msg error");
                return nmea;
            }
        }
        nmea = pb_nmea.data();
        return nmea;
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        m_client->cbLocation = cb;
        mifsa::ota::pb::Command pb_command;
        pb_command.set_type(mifsa::ota::pb::Command_Type_START_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        m_client->invoke(mifsa::ota::pb::MSG_COMMAND, builder, _time_out);
    }
    virtual void stopNavigation() override
    {
        mifsa::ota::pb::Command pb_command;
        pb_command.set_type(mifsa::ota::pb::Command_Type_STOP_NAVIGATION);
        CFdbProtoMsgBuilder builder(pb_command);
        m_client->invoke(mifsa::ota::pb::MSG_COMMAND, builder, _time_out);
    }

private:
    std::unique_ptr<FdbusClient> m_client;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_INTERFACE_REALIZE_FDBUS_H
