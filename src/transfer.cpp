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

#include "mifsa/ota/transfer.h"
#include "mifsa/ota/upgrade.h"
#include "mifsa/utils/string.h"
#include <algorithm>
#include <iostream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {

bool Transfer::operator==(const Transfer& transfer) const noexcept
{
    return domain == transfer.domain
        && name == transfer.name
        && total == transfer.total;
}

bool Transfer::operator!=(const Transfer& transfer) const noexcept
{
    return !(*this == transfer);
}

bool Transfer::operator<(const Transfer& transfer) const noexcept
{
    if (total == 0 || transfer.total == 0) {
        return false;
    }
    if ((double)current / total > (double)transfer.current / transfer.total) {
        return true;
    };
    return false;
}

bool Transfer::operator>(const Transfer& transfer) const noexcept
{
    if (total == 0 || transfer.total == 0) {
        return false;
    }
    if ((double)current / total < (double)transfer.current / transfer.total) {
        return true;
    };
    return false;
}

std::ostream& operator<<(std::ostream& ostream, const Transfer& transfer) noexcept
{
    ostream << "[progress]: " << Utils::doubleToString(transfer.progress) << " %, ";
    if (!transfer.domain.empty()) {
        ostream << "[domain]: " << transfer.domain << ", ";
    }
    ostream << "[name]: " << transfer.name << ", ";
    ostream << "[speed]: " << File::getSizeStr(transfer.speed) << "/S, ";
    ostream << "[total]: " << File::getSizeStr(transfer.total) << ", ";
    ostream << "[current]: " << File::getSizeStr(transfer.current) << ", ";
    ostream << "[pass]: " << transfer.pass << " s, ";
    ostream << "[left]: " << transfer.left << " s";
    return ostream;
}

Transfers::~Transfers()
{
    clear();
    shrink_to_fit();
}

Transfer* Transfers::update(Transfer&& transfer, bool force) noexcept
{
    for (Transfer& t : *this) {
        if (t == transfer) {
            t = transfer;
            return &t;
        }
    }
    if (force) {
        push_back(std::move(transfer));
        return &(this->back());
    }
    return nullptr;
}

void Transfers::sort() noexcept
{
    std::sort(begin(), end(), [](const Transfer& lhs, const Transfer& rhs) {
        return lhs < rhs;
    });
}

std::ostream& operator<<(std::ostream& ostream, const Transfers& transfers) noexcept
{
    if (transfers.empty()) {
        return ostream;
    }
    for (const Transfer& transfer : transfers) {
        ostream << transfer << "\n";
    }
    return ostream;
}

}

MIFSA_NAMESPACE_END
