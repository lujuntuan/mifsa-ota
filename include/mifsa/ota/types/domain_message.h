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

#ifndef MIFSA_OTA_TYPES_DOMAIN_MESSAGE_H
#define MIFSA_OTA_TYPES_DOMAIN_MESSAGE_H

#include "domain.h"
#include <mifsa/base/variant.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct DomainMessage final {
    DomainMessage() = default;
    template <typename T>
    explicit DomainMessage(T&& _domain, bool _discovery) noexcept
        : domain(std::forward<T>(_domain))
        , discovery(_discovery)
    {
    }
    Domain domain;
    bool discovery = false;
};
}
VARIANT_DECLARE_TYPE(Ota::DomainMessage, ota_domain_message);

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_TYPES_DOMAIN_MESSAGE_H
