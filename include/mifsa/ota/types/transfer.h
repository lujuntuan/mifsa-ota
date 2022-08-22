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

#ifndef MIFSA_OTA_TYPES_TRANSFER_H
#define MIFSA_OTA_TYPES_TRANSFER_H

#include "mifsa/base/define.h"
#include "mifsa/base/variant.h"
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct MIFSA_EXPORT Transfer final {
    std::string domain;
    std::string name;
    float progress = 0;
    uint32_t total = 0;
    uint32_t current = 0;
    uint32_t speed = 0;
    uint32_t pass = 0;
    uint32_t left = 0;

public:
    bool operator==(const Transfer& transfer) const noexcept;
    bool operator!=(const Transfer& transfer) const noexcept;
    bool operator<(const Transfer& transfer) const noexcept;
    bool operator>(const Transfer& transfer) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Transfer& transfer) noexcept;
};

class MIFSA_EXPORT Transfers final : public std::vector<Transfer> {
public:
    ~Transfers();
    Transfer* update(Transfer&& transfer, bool force = false) noexcept;
    void sort() noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Transfers& transfers) noexcept;
};
}
VARIANT_DECLARE_TYPE(Ota::Transfer, ota_transfer);
VARIANT_DECLARE_TYPE(Ota::Transfers, ota_transfers);

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_TYPES_TRANSFER_H
