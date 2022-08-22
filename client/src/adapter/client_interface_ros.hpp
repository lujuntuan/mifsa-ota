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

#ifdef MIFSA_SUPPORT_ROS

#include "mifsa/ota/client.h"
#include <mifsa/base/thread.h>
#include <mifsa_ota_idl/msg/command.hpp>
#include <mifsa_ota_idl/msg/location.hpp>
#include <mifsa_ota_idl/srv/nmea.hpp>
#include <rclcpp/rclcpp.hpp>

using namespace mifsa_ota_idl;

MIFSA_NAMESPACE_BEGIN

int _time_out = 5000;

namespace Ota {

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
            m_locationSub = m_node->create_subscription<msg::Location>("/mifsa/ota/location", qosConfig, [this](msg::Location::UniquePtr ros_location) {
                if (cbLocation) {
                    Location location;
                    location.size = ros_location->size;
                    location.flags = ros_location->flags;
                    location.latitude = ros_location->latitude;
                    location.longitude = ros_location->longitude;
                    location.altitude = ros_location->altitude;
                    location.speed = ros_location->speed;
                    location.bearing = ros_location->bearing;
                    location.accuracy = ros_location->accuracy;
                    location.timestamp = ros_location->timestamp;
                    location.data = ros_location->data;
                    cbLocation(location);
                }
            });
            m_commandPub = m_node->create_publisher<msg::Command>("/mifsa/ota/command", qosConfig);
            m_nmeaClient = m_node->create_client<srv::Nmea>("mifsa_ota_server_nmea", ::rmw_qos_profile_default, m_callbackGroup);
            m_serverIsReady = m_nmeaClient->service_is_ready();
            if (m_serverIsReady) {
                _cbConnected(m_serverIsReady);
            }
            m_timer = m_node->create_wall_timer(std::chrono::milliseconds(500), [this]() {
                bool ready = m_nmeaClient->service_is_ready();
                if (m_serverIsReady != ready) {
                    m_serverIsReady = ready;
                    _cbConnected(m_serverIsReady);
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
    virtual std::string version() override
    {
        return MIFSA_OTA_VERSION;
    }
    virtual bool connected() override
    {
        return m_nmeaClient->service_is_ready();
    }
    virtual std::string getNmea() override
    {
        auto ros_nmeaReq = std::make_shared<srv::Nmea::Request>();
        ros_nmeaReq->type = srv::Nmea::Request::QUERY_NMEA;
        auto result = m_nmeaClient->async_send_request(ros_nmeaReq);
        auto status = result.wait_for(std::chrono::milliseconds(_time_out));
        if (status == std::future_status::ready) {
            return result.get()->data;
        } else {
            LOG_WARNING("invoke failed");
            return "";
        }
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        cbLocation = cb;
        msg::Command ros_command;
        ros_command.type = msg::Command::START_NAVIGATION;
        m_commandPub->publish(ros_command);
    }
    virtual void stopNavigation() override
    {
        msg::Command ros_command;
        ros_command.type = msg::Command::STOP_NAVIGATION;
        m_commandPub->publish(ros_command);
    }

private:
    Thread m_thread;
    rclcpp::Node::SharedPtr m_node;
    rclcpp::CallbackGroup::SharedPtr m_callbackGroup;
    rclcpp::Subscription<msg::Location>::SharedPtr m_locationSub;
    rclcpp::Publisher<msg::Command>::SharedPtr m_commandPub;
    rclcpp::Client<srv::Nmea>::SharedPtr m_nmeaClient;
    rclcpp::TimerBase::SharedPtr m_timer;
    bool m_serverIsReady = false;
    //
    CbLocation cbLocation;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_CLIENT_INTERFACE_ROS_H
