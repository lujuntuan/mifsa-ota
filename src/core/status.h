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

#ifndef MIFSA_OTA_CORE_STATUS_H
#define MIFSA_OTA_CORE_STATUS_H

#include <functional>
#include <mifsa/base/define.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Core {
    using BreakFunction = std::function<bool()>;
    enum State {
        SUCCEED = 0,
        FAILED,
        CANCELED,
    };
    struct Status {
        Status() = default;
        explicit Status(State _state, int _error)
            : state(_state)
            , error(_error)
        {
        }
        State state = SUCCEED;
        int error = 0;
    };
}
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_CORE_STATUS_H
