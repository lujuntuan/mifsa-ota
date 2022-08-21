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

#ifndef MIFSA_OTA_PROVIDER_REALIZE_VSOMEIP_H
#define MIFSA_OTA_PROVIDER_REALIZE_VSOMEIP_H

#ifdef MIFSA_SUPPORT_VSOMEIP

#include "mifsa/ota/provider.h"
#include <CommonAPI/CommonAPI.hpp>
#include <mifsa/utils/dir.h>
#include <mifsa/utils/host.h>
#include <v1/commonapi/mifsa/ota/interfacesStubDefault.hpp>

using namespace v1_0::commonapi::mifsa::ota;

MIFSA_NAMESPACE_BEGIN

namespace Ota {

class interfacesImpl : public interfacesStubDefault {
public:
    using interfacesStubDefault::interfacesStubDefault;
    virtual void startNavigation(const std::shared_ptr<CommonAPI::ClientId> _client) override
    {
        if (cbStartNavigation) {
            cbStartNavigation();
        }
    }
    virtual void stopNavigation(const std::shared_ptr<CommonAPI::ClientId> _client) override
    {
        if (cbStopNavigation) {
            cbStopNavigation();
        }
    }
    virtual void getNmea(const std::shared_ptr<CommonAPI::ClientId> _client, getNmeaReply_t _reply) override
    {
        std::string nmea;
        if (cbNmea) {
            cbNmea(nmea);
        }
        _reply(nmea);
    }

public:
    CbNmea cbNmea;
    CbStartNavigation cbStartNavigation;
    CbStopNavigation cbStopNavigation;
};

class ProviderImplementation : public Provider {
public:
    ProviderImplementation()
    {
        std::string vsomeipApiCfg = Utils::getCfgPath("vsomeip_mifsa_ota_server.json", "VSOMEIP_CONFIGURATION", "mifsa");
        if (!vsomeipApiCfg.empty()) {
            Utils::setEnvironment("VSOMEIP_CONFIGURATION", vsomeipApiCfg);
        }
        m_commonApiStub = std::make_shared<interfacesImpl>();
        CommonAPI::Runtime::get()->registerService("local", "commonapi.mifsa.ota.interfaces", m_commonApiStub, "mifsa_ota_server");
    }
    ~ProviderImplementation()
    {
    }
    virtual void setCbNmea(const CbNmea& cb) override
    {
        m_commonApiStub->cbNmea = cb;
    }
    virtual void reportOta(const Location& location) override
    {
        interfaces::Location capi_location;
        capi_location.setSize(location.size);
        capi_location.setFlags(location.flags);
        capi_location.setLatitude(location.latitude);
        capi_location.setLongitude(location.longitude);
        capi_location.setAltitude(location.altitude);
        capi_location.setSpeed(location.speed);
        capi_location.setBearing(location.bearing);
        capi_location.setAccuracy(location.accuracy);
        capi_location.setTimestamp(location.timestamp);
        capi_location.setData(location.data);
        m_commonApiStub->fireReportLocationEvent(capi_location);
    }
    virtual void setCbStartNavigation(const CbStartNavigation& cb) override
    {
        m_commonApiStub->cbStartNavigation = cb;
    }
    virtual void setCbStopNavigation(const CbStopNavigation& cb) override
    {
        m_commonApiStub->cbStopNavigation = cb;
    }

private:
    std::shared_ptr<interfacesImpl> m_commonApiStub;
};

}

MIFSA_NAMESPACE_END

#endif

#endif // MIFSA_OTA_PROVIDER_REALIZE_VSOMEIP_H
