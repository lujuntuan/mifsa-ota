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

#ifndef MIFSA_OTA_WEB_EVENT_H
#define MIFSA_OTA_WEB_EVENT_H

#include "mifsa/ota/setting.h"
#include "web_feed.h"
#include "web_init.h"
#include <mifsa/base/event.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class WebEvent : public Event {
public:
    enum WebEventType {
        FUNCTION = 0,
        REQ_INIT,
        REQ_CHECK,
        REQ_DOWNLOAD,
        REQ_VERIFY,
        REQ_DISTRIBUTE,
        REQ_STOP,
        REQ_CLEAR,
        REQ_CLEARALL,
        REQ_FEEDBACK,
    };
    explicit WebEvent(WebEventType type) noexcept
        : Event(MIFSA_QUEUE_ID_WEB, type)
    {
    }
};

class WebInitEvent : public WebEvent {
public:
    explicit WebInitEvent(const WebInit& webInit) noexcept
        : WebEvent(REQ_INIT)
        , m_webInit(webInit)
    {
    }

public:
    inline const WebInit& webInit() const noexcept
    {
        return m_webInit;
    }

private:
    WebInit m_webInit;
};

class WebFeedEvent : public WebEvent {
public:
    explicit WebFeedEvent(const WebFeed& webFeed) noexcept
        : WebEvent(REQ_FEEDBACK)
        , m_webFeed(webFeed)
    {
    }

public:
    inline const WebFeed& webFeed() const noexcept
    {
        return m_webFeed;
    }

private:
    WebFeed m_webFeed;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_WEB_EVENT_H
