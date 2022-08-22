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

#ifndef MIFSA_OTA_TYPES_DETAIL_MESSAGE_H
#define MIFSA_OTA_TYPES_DETAIL_MESSAGE_H

#include "domain.h"
#include <mifsa/base/variant.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct DetailMessage final {
    DetailMessage() = default;
    template <typename T1, typename T2>
    explicit DetailMessage(uint32_t _id, ServerState _state, ServerState _last, bool _active, int _error, float _step, float _progress, T1&& _messgae, T2&& _details) noexcept
        : id(_id)
        , state(_state)
        , last(_last)
        , active(_active)
        , error(_error)
        , step(_step)
        , progress(_progress)
        , message(std::forward<T1>(_messgae))
        , details(std::forward<T2>(_details))
    {
    }
    uint32_t id = 0;
    ServerState state = MR_UNKNOWN;
    ServerState last = MR_UNKNOWN;
    bool active = false;
    int error = 0;
    float step = .0f;
    float progress = .0f;
    std::string message;
    Details details;

public:
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const DetailMessage& detailMessage) noexcept;
};
}
VARIANT_DECLARE_TYPE(Ota::DetailMessage, ota_detail_message);

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_TYPES_DETAIL_MESSAGE_H
