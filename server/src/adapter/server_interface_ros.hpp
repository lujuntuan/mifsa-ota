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

#ifndef MIFSA_OTA_SERVER_INTERFACE_ROS_H
#define MIFSA_OTA_SERVER_INTERFACE_ROS_H

#include "server.h"
#include <mifsa/base/semaphore.h>
#include <mifsa/base/thread.h>
#include <mifsa_ota_idl/msg/control_message.hpp>
#include <mifsa_ota_idl/msg/detail_message.hpp>
#include <mifsa_ota_idl/msg/domain_message.hpp>
#include <rclcpp/rclcpp.hpp>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
static mifsa_ota_idl::msg::Package _getPackage(const Package& package)
{
    mifsa_ota_idl::msg::Package t_package;
    t_package.domain = package.domain;
    t_package.part = package.part;
    t_package.version = package.version;
    t_package.meta = package.meta.toJson();
    for (const auto& file : package.files) {
        mifsa_ota_idl::msg::File t_file;
        t_file.domain = file.domain;
        t_file.name = file.name;
        t_file.url = file.url;
        t_file.size = file.size;
        t_file.md5 = file.md5;
        t_file.sha1 = file.sha1;
        t_file.sha256 = file.sha256;
        t_package.files.push_back(std::move(t_file));
    }
    return t_package;
}

static mifsa_ota_idl::msg::ControlMessage _getControlMessage(const ControlMessage& controlMessage)
{
    mifsa_ota_idl::msg::ControlMessage t_controlMessage;
    t_controlMessage.id = controlMessage.id;
    t_controlMessage.control = controlMessage.control;
    t_controlMessage.upgrade.id = controlMessage.upgrade.id;
    t_controlMessage.upgrade.download = controlMessage.upgrade.download;
    t_controlMessage.upgrade.deploy = controlMessage.upgrade.deploy;
    t_controlMessage.upgrade.maintenance = controlMessage.upgrade.maintenance;
    for (const auto& package : controlMessage.upgrade.packages) {
        t_controlMessage.upgrade.packages.push_back(_getPackage(package));
    }
    t_controlMessage.depends = controlMessage.depends;
    return t_controlMessage;
}

static mifsa_ota_idl::msg::DetailMessage _getDetailMessage(const DetailMessage& detailMessage)
{
    mifsa_ota_idl::msg::DetailMessage t_detailMessage;
    t_detailMessage.id = detailMessage.id;
    t_detailMessage.state = detailMessage.state;
    t_detailMessage.last = detailMessage.last;
    t_detailMessage.active = detailMessage.active;
    t_detailMessage.error = detailMessage.error;
    t_detailMessage.step = detailMessage.step;
    t_detailMessage.progress = detailMessage.progress;
    t_detailMessage.message = detailMessage.message;
    for (const auto& d : detailMessage.details) {
        mifsa_ota_idl::msg::Detail t_detail;
        t_detail.domain.name = d.domain.name;
        t_detail.domain.guid = d.domain.guid;
        t_detail.domain.state = d.domain.state;
        t_detail.domain.last = d.domain.last;
        t_detail.domain.watcher = d.domain.watcher;
        t_detail.domain.error = d.domain.error;
        t_detail.domain.version = d.domain.version;
        t_detail.domain.attribute = d.domain.attribute.toJson();
        t_detail.domain.meta = d.domain.meta.toJson();
        t_detail.domain.progress = d.domain.progress;
        t_detail.domain.message = d.domain.message;
        t_detail.domain.answer = d.domain.answer;
        t_detail.package = _getPackage(d.package);
        for (const auto& transfer : d.transfers) {
            mifsa_ota_idl::msg::Transfer t_transfer;
            t_transfer.domain = transfer.domain;
            t_transfer.name = transfer.name;
            t_transfer.progress = transfer.progress;
            t_transfer.speed = transfer.speed;
            t_transfer.total = transfer.total;
            t_transfer.current = transfer.current;
            t_transfer.pass = transfer.pass;
            t_transfer.left = transfer.left;
            t_detail.transfers.push_back(std::move(t_transfer));
        }
        t_detail.progress = d.progress;
        t_detail.deploy = d.deploy.get();
        t_detailMessage.details.push_back(std::move(t_detail));
    }
    return t_detailMessage;
}

static DomainMessage _getDomainMessage(const mifsa_ota_idl::msg::DomainMessage::UniquePtr& t_domainMessage)
{
    DomainMessage domainMessage;
    domainMessage.domain.name = t_domainMessage->domain.name;
    domainMessage.domain.guid = t_domainMessage->domain.guid;
    domainMessage.domain.state = (ClientState)t_domainMessage->domain.state;
    domainMessage.domain.last = (ClientState)t_domainMessage->domain.last;
    domainMessage.domain.watcher = t_domainMessage->domain.watcher;
    domainMessage.domain.error = t_domainMessage->domain.error;
    domainMessage.domain.version = t_domainMessage->domain.version;
    domainMessage.domain.attribute = Variant::readJson(t_domainMessage->domain.attribute);
    domainMessage.domain.meta = Variant::readJson(t_domainMessage->domain.meta);
    domainMessage.domain.progress = t_domainMessage->domain.progress;
    domainMessage.domain.message = t_domainMessage->domain.message;
    domainMessage.domain.answer = (Answer)t_domainMessage->domain.answer;
    domainMessage.discovery = t_domainMessage->discovery;
    return domainMessage;
}
class ServerInterfaceAdapter : public ServerInterface {
public:
    ServerInterfaceAdapter()
    {
        Semaphore sema;
        m_thread.start([&]() {
            rclcpp::init(0, nullptr);
            m_node = rclcpp::Node::make_shared("mifsa_ota_server");
            m_callbackGroup = m_node->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
            auto qosConfig = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
            m_control = m_node->create_publisher<mifsa_ota_idl::msg::ControlMessage>("/mifsa/ota/control", qosConfig);
            m_detail = m_node->create_publisher<mifsa_ota_idl::msg::DetailMessage>("/mifsa/ota/detail", qosConfig);
            m_domain = m_node->create_subscription<mifsa_ota_idl::msg::DomainMessage>("/mifsa/ota/domain", qosConfig,
                [this](mifsa_ota_idl::msg::DomainMessage::UniquePtr t_domainMessage) {
                    const auto& domainMessage = _getDomainMessage(t_domainMessage);
                    if (m_cbDomain) {
                        m_cbDomain(domainMessage);
                    }
                });
            sema.reset();
            rclcpp::executors::MultiThreadedExecutor exec;
            exec.add_node(m_node);
            exec.spin();
        });
        sema.acquire();
    }
    ~ServerInterfaceAdapter()
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
    virtual void sendControlMessage(const ControlMessage& controlMessage) override
    {
        const auto& t_controlMessage = _getControlMessage(controlMessage);
        m_control->publish(t_controlMessage);
    }
    virtual void sendDetailMessage(const DetailMessage& detailMessage) override
    {
        const auto& t_detailMessage = _getDetailMessage(detailMessage);
        m_detail->publish(t_detailMessage);
    }
    virtual void setCbReportDomain(const CbDomain& cb) override
    {
        m_cbDomain = cb;
    }

private:
    Thread m_thread;
    rclcpp::Node::SharedPtr m_node;
    rclcpp::CallbackGroup::SharedPtr m_callbackGroup;
    rclcpp::Publisher<mifsa_ota_idl::msg::ControlMessage>::SharedPtr m_control;
    rclcpp::Publisher<mifsa_ota_idl::msg::DetailMessage>::SharedPtr m_detail;
    rclcpp::Subscription<mifsa_ota_idl::msg::DomainMessage>::SharedPtr m_domain;
    CbDomain m_cbDomain;
};

}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_SERVER_INTERFACE_ROS_H
