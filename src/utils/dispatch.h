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

#ifndef MIFSA_UTILS_DISPATCH_H
#define MIFSA_UTILS_DISPATCH_H

#include "mifsa/base/define.h"
#include <functional>
#include <string>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Utils {
    extern MIFSA_EXPORT bool dispatch(const std::string& oldFile, const std::string& newFile, const std::string& patchFile,
        const std::function<bool()>& breakFunction = nullptr, const std::function<void(size_t, size_t)>& progressFunction = nullptr);
}
}

MIFSA_NAMESPACE_END

#endif // MIFSA_UTILS_DISPATCH_H
