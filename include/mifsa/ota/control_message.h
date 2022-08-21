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

#ifndef MIFSA_CONTROL_MESSAGE_H
#define MIFSA_CONTROL_MESSAGE_H

#include "mifsa/ota/domain.h"
#include "mifsa/ota/upgrade.h"
#include <mifsa/base/variant.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct ControlMessage final {
    ControlMessage() = default;
    template <typename T1, typename T2>
    explicit ControlMessage(uint32_t _id, Control _control, T1&& _upgrade, T2&& _depends) noexcept
        : id(_id)
        , control(_control)
        , upgrade(std::forward<T1>(_upgrade))
        , depends(std::forward<T2>(_depends))
    {
    }
    uint32_t id = 0;
    Control control = CTL_UNKNOWN;
    Upgrade upgrade;
    Depends depends;

public:
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const ControlMessage& controlMessage) noexcept;
};
}
VARIANT_DECLARE_TYPE(Ota::ControlMessage, ota_control_message);

MIFSA_NAMESPACE_END

#endif // MIFSA_CONTROL_MESSAGE_H
