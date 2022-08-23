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

#ifndef MIFSA_OTA_CLIENT_INTERFACE_ROS_H
#define MIFSA_OTA_CLIENT_INTERFACE_ROS_H

#include "mifsa/ota/client.h"
#include <mifsa/base/thread.h>
#include <mifsa_ota_idl/msg/control_message.hpp>
#include <mifsa_ota_idl/msg/detail_message.hpp>
#include <mifsa_ota_idl/msg/domain_message.hpp>
#include <rclcpp/rclcpp.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
static Package _getPackage(const mifsa_ota_idl::msg::Package& t_package)
{
    Package package;
    package.domain = t_package.domain;
    package.part = t_package.part;
    package.version = t_package.version;
    package.meta = Variant::readJson(t_package.meta);
    for (const auto& msg_file : t_package.files) {
        File file;
        file.domain = msg_file.domain;
        file.name = msg_file.name;
        file.url = msg_file.url;
        file.size = msg_file.size;
        file.md5 = msg_file.md5;
        file.sha1 = msg_file.sha1;
        file.sha256 = msg_file.sha256;
        package.files.push_back(std::move(file));
    }
    return package;
}

static ControlMessage _getControlMessage(const mifsa_ota_idl::msg::ControlMessage::UniquePtr& t_controlMessage)
{
    ControlMessage controlMessage;
    controlMessage.id = t_controlMessage->id;
    controlMessage.control = (Control)t_controlMessage->control;
    controlMessage.upgrade.id = t_controlMessage->upgrade.id;
    controlMessage.upgrade.download = (Upgrade::Method)t_controlMessage->upgrade.download;
    controlMessage.upgrade.deploy = (Upgrade::Method)t_controlMessage->upgrade.deploy;
    controlMessage.upgrade.maintenance = t_controlMessage->upgrade.maintenance;
    for (const auto& msg_package : t_controlMessage->upgrade.packages) {
        const auto& package = _getPackage(msg_package);
        controlMessage.upgrade.packages.push_back(std::move(package));
    }
    controlMessage.depends = t_controlMessage->depends;
    return controlMessage;
}

static DetailMessage _getDetailMessage(const mifsa_ota_idl::msg::DetailMessage::UniquePtr& t_detailMessage)
{
    DetailMessage detailMessage;
    detailMessage.id = (ServerState)t_detailMessage->id;
    detailMessage.state = (ServerState)t_detailMessage->state;
    detailMessage.last = (ServerState)t_detailMessage->last;
    detailMessage.active = t_detailMessage->active;
    detailMessage.error = t_detailMessage->error;
    detailMessage.step = t_detailMessage->step;
    detailMessage.progress = t_detailMessage->progress;
    detailMessage.message = t_detailMessage->message;
    for (const auto& msg_detail : t_detailMessage->details) {
        Domain domain(msg_detail.domain.name, msg_detail.domain.guid);
        domain.state = (ClientState)msg_detail.domain.state;
        domain.last = (ClientState)msg_detail.domain.last;
        domain.watcher = msg_detail.domain.watcher;
        domain.error = msg_detail.domain.error;
        domain.version = msg_detail.domain.version;
        domain.attribute = Variant::readJson(msg_detail.domain.attribute);
        domain.meta = Variant::readJson(msg_detail.domain.meta);
        domain.progress = msg_detail.domain.progress;
        domain.message = msg_detail.domain.message;
        domain.answer = (Answer)msg_detail.domain.answer;
        Detail detail(std::move(domain));
        const auto& package = _getPackage(msg_detail.package);
        detail.package = package;
        for (const auto& msg_transfer : msg_detail.transfers) {
            Transfer transfer;
            transfer.domain = msg_transfer.domain;
            transfer.name = msg_transfer.name;
            transfer.progress = msg_transfer.progress;
            transfer.speed = msg_transfer.speed;
            transfer.total = msg_transfer.total;
            transfer.current = msg_transfer.current;
            transfer.pass = msg_transfer.pass;
            transfer.left = msg_transfer.left;
            detail.transfers.push_back(std::move(transfer));
        }
        detail.progress = msg_detail.progress;
        if (msg_detail.deploy > 0) {
            detail.deploy.start(Elapsed::current() - msg_detail.deploy);
        }
        detailMessage.details.push_back(std::move(detail));
    }
    return detailMessage;
}

static mifsa_ota_idl::msg::DomainMessage _getDomainMessage(const DomainMessage& domainMessage)
{
    mifsa_ota_idl::msg::DomainMessage t_domainMessage;
    t_domainMessage.domain.name = domainMessage.domain.name;
    t_domainMessage.domain.guid = domainMessage.domain.guid;
    t_domainMessage.domain.state = domainMessage.domain.state;
    t_domainMessage.domain.last = domainMessage.domain.last;
    t_domainMessage.domain.watcher = domainMessage.domain.watcher;
    t_domainMessage.domain.error = domainMessage.domain.error;
    t_domainMessage.domain.version = domainMessage.domain.version;
    t_domainMessage.domain.attribute = domainMessage.domain.attribute.toJson();
    t_domainMessage.domain.meta = domainMessage.domain.meta.toJson();
    t_domainMessage.domain.progress = domainMessage.domain.progress;
    t_domainMessage.domain.message = domainMessage.domain.message;
    t_domainMessage.domain.answer = domainMessage.domain.answer;
    t_domainMessage.discovery = domainMessage.discovery;
    return t_domainMessage;
}

class ClientInterfaceAdapter : public ClientInterface {
public:
    ClientInterfaceAdapter()
    {
        Semaphore sema;
        m_thread.start([&]() {
            rclcpp::init(0, nullptr);
            m_node = rclcpp::Node::make_shared("mifsa_ota_client");
            m_callbackGroup = m_node->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
            auto qosConfig = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
            m_control = m_node->create_subscription<mifsa_ota_idl::msg::ControlMessage>("/mifsa/ota/control", qosConfig,
                [this](mifsa_ota_idl::msg::ControlMessage::UniquePtr t_controlMessage) {
                    if (checkControlMessageId && checkControlMessageId(t_controlMessage->id)) {
                        return;
                    }
                    const auto& controlMessage = _getControlMessage(t_controlMessage);
                    if (m_cbControlMessage) {
                        m_cbControlMessage(controlMessage);
                    }
                });
            m_domain = m_node->create_publisher<mifsa_ota_idl::msg::DomainMessage>("/mifsa/ota/domain", qosConfig);
            m_connected = connected();
            if (m_connected) {
                cbConnected(m_connected);
            }
            m_timer = m_node->create_wall_timer(std::chrono::milliseconds(500), [this]() {
                bool isConnected = connected();
                if (m_connected != isConnected) {
                    m_connected = isConnected;
                    cbConnected(m_connected);
                }
            });
            sema.reset();
            rclcpp::executors::MultiThreadedExecutor exec;
            exec.add_node(m_node);
            exec.spin();
        });
        sema.acquire();
    }
    ~ClientInterfaceAdapter()
    {
        rclcpp::shutdown();
        m_thread.stop();
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
        return m_control->get_publisher_count() > 0;
    }
    virtual void setCbControlMessage(CbControlMessage cb) override
    {
        m_cbControlMessage = cb;
    }
    virtual void setCbDetailMessage(CbDetailMessage cb) override
    {
        m_cbDetailMessage = cb;
        auto qosConfig = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
        m_detail = m_node->create_subscription<mifsa_ota_idl::msg::DetailMessage>("/mifsa/ota/detail", qosConfig,
            [this](mifsa_ota_idl::msg::DetailMessage::UniquePtr t_detailMessage) {
                if (checkDetailMessageId && checkDetailMessageId(t_detailMessage->id)) {
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
        const auto& t_domainMessage = _getDomainMessage(domainMessage);
        m_domain->publish(t_domainMessage);
        return true;
    }

private:
    Thread m_thread;
    rclcpp::Node::SharedPtr m_node;
    rclcpp::CallbackGroup::SharedPtr m_callbackGroup;
    rclcpp::Subscription<mifsa_ota_idl::msg::ControlMessage>::SharedPtr m_control;
    rclcpp::Subscription<mifsa_ota_idl::msg::DetailMessage>::SharedPtr m_detail;
    rclcpp::Publisher<mifsa_ota_idl::msg::DomainMessage>::SharedPtr m_domain;
    rclcpp::TimerBase::SharedPtr m_timer;
    bool m_connected = false;
    CbControlMessage m_cbControlMessage;
    CbDetailMessage m_cbDetailMessage;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CLIENT_INTERFACE_ROS_H
