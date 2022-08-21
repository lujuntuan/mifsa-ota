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

#ifndef MIFSA_CLIENT_EVENT_H
#define MIFSA_CLIENT_EVENT_H

#include "mifsa/base/event.h"
#include "mifsa/ota/detail_message.h"
#include "mifsa/ota/domain.h"
#include "mifsa/ota/setting.h"
#include "mifsa/ota/upgrade.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
class ClientEvent : public Event {
public:
    enum ClientEventType {
        FUNCTION = 0,
        REQ_CONTROL,
        REQ_DETAIL,
        RES_ERROR,
        RES_DOWNLOAD,
        RES_VERIFY,
        RES_PATCH,
        RES_ANSWER,
        RES_TRANSFER_PROGRESS,
        RES_DEPLOY_DONE,
        RES_CANCEL_DONE,
        RES_DEPLOY_PROGRESS,
    };
    explicit ClientEvent(ClientEventType type, const VariantMap& data = VariantMap()) noexcept
        : Event(MIFSA_QUEUE_ID_CLIENT, type)
        , m_data(data)
    {
    }
    inline const VariantMap& data() const noexcept
    {
        return m_data;
    }

private:
    VariantMap m_data;
};

class ClientControlEvent : public ClientEvent {
public:
    template <typename T1, typename T2>
    explicit ClientControlEvent(Control control, T1&& upgrade, T2&& depends) noexcept
        : ClientEvent(REQ_CONTROL)
        , m_control(control)
        , m_upgrade(std::forward<T1>(upgrade))
        , m_depends(std::forward<T2>(depends))
    {
    }
    inline Control control() const noexcept
    {
        return m_control;
    }
    inline const Upgrade& upgrade() const noexcept
    {
        return m_upgrade;
    }
    inline const Depends& depends() const noexcept
    {
        return m_depends;
    }

private:
    Control m_control;
    Upgrade m_upgrade;
    Depends m_depends;
};

class ClientDetailEvent : public ClientEvent {
public:
    template <typename T>
    explicit ClientDetailEvent(T&& detail) noexcept
        : ClientEvent(REQ_DETAIL)
        , m_detailMessage(std::forward<T>(detail))
    {
    }
    inline const DetailMessage& detailMessage() const noexcept
    {
        return m_detailMessage;
    }

private:
    DetailMessage m_detailMessage;
};

class ClientTransferEvent : public ClientEvent {
public:
    template <typename T>
    explicit ClientTransferEvent(T&& transfers) noexcept
        : ClientEvent(RES_TRANSFER_PROGRESS)
        , m_transfers(std::forward<T>(transfers))
    {
    }
    inline const Transfers& transfers() const noexcept
    {
        return m_transfers;
    }

private:
    Transfers m_transfers;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_CLIENT_EVENT_H
