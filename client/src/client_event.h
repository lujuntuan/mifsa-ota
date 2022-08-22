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

#include "core/setting.h"
#include "mifsa/base/event.h"
#include "mifsa/ota/types/control_message.h"
#include "mifsa/ota/types/detail_message.h"

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
        : Event(MIFSA_OTA_QUEUE_ID_CLIENT, type)
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
    template <typename T>
    explicit ClientControlEvent(T&& controlMessage) noexcept
        : ClientEvent(REQ_CONTROL)
        , m_controlMessage(controlMessage)
    {
    }
    inline ControlMessage controlMessage() const noexcept
    {
        return m_controlMessage;
    }

private:
    ControlMessage m_controlMessage;
};

class ClientDetailEvent : public ClientEvent {
public:
    template <typename T>
    explicit ClientDetailEvent(T&& detailMessage) noexcept
        : ClientEvent(REQ_DETAIL)
        , m_detailMessage(std::forward<T>(detailMessage))
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
