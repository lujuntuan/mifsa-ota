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

#ifdef MIFSA_SUPPORT_VSOMEIP

#ifndef MIFSA_OTA_INTERFACE_REALIZE_H
#define MIFSA_OTA_INTERFACE_REALIZE_H

#include "mifsa/ota/interface.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa/ota/interfacesProxy.hpp>

using namespace v1_0::commonapi::mifsa::ota;

MIFSA_NAMESPACE_BEGIN

CommonAPI::CallInfo _callInfo(5000);

namespace Ota {

class InterfaceImplementation : public Interface {
public:
    InterfaceImplementation()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_client.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        m_commonApiProxy = CommonAPI::Runtime::get()->buildProxy<interfacesProxy>("local", "commonapi.mifsa.ota.interfaces", "mifsa_ota_client");
        m_commonApiProxy->getProxyStatusEvent().subscribe([this](const CommonAPI::AvailabilityStatus& status) {
            if (status == CommonAPI::AvailabilityStatus::AVAILABLE) {
                _cbConnected(true);
            } else {
                _cbConnected(false);
            }
        });
        m_commonApiProxy->getReportLocationEvent().subscribe([this](const interfaces::Location& capi_location) {
            if (cbLocation) {
                Location location;
                location.size = capi_location.getSize();
                location.flags = capi_location.getFlags();
                location.latitude = capi_location.getLatitude();
                location.longitude = capi_location.getLongitude();
                location.altitude = capi_location.getAltitude();
                location.speed = capi_location.getSpeed();
                location.bearing = capi_location.getBearing();
                location.accuracy = capi_location.getAccuracy();
                location.timestamp = capi_location.getTimestamp();
                location.data = capi_location.getData();
                cbLocation(location);
            }
        });
        {
        }
    }
    ~InterfaceImplementation()
    {
    }
    virtual std::string version() override
    {
        return MIFSA_OTA_VERSION;
    }
    virtual bool connected() override
    {
        return m_commonApiProxy->isAvailable();
    }
    virtual std::string getNmea() override
    {
        CommonAPI::CallStatus status;
        interfaces::Nmea nmea;
        m_commonApiProxy->getNmea(status, nmea, &_callInfo);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
        return nmea.getData();
    }
    virtual void startNavigation(const CbLocation& cb) override
    {
        cbLocation = cb;
        CommonAPI::CallStatus status;
        m_commonApiProxy->startNavigation(status);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
    }
    virtual void stopNavigation() override
    {
        CommonAPI::CallStatus status;
        m_commonApiProxy->stopNavigation(status);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            LOG_WARNING("invoke failed, error code=", (int)status);
        }
    }

private:
    std::shared_ptr<interfacesProxy<>> m_commonApiProxy;
    CbLocation cbLocation;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_INTERFACE_REALIZE_H
