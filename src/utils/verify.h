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

#ifndef MIFSA_OTA_UTILS_VERIFY_H
#define MIFSA_OTA_UTILS_VERIFY_H

#include "mifsa/base/define.h"
#include <functional>
#include <string>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Utils {
    extern MIFSA_EXPORT std::string getStrMd5(const std::string& srcStr);
    extern MIFSA_EXPORT std::string getStrSha1(const std::string& srcStr);
    extern MIFSA_EXPORT std::string getStrSha256(const std::string& srcStr);
    extern MIFSA_EXPORT std::string getFileMd5(const std::string& filePath, const std::function<bool()>& breakFunction = nullptr, const std::function<void(size_t, size_t)>& progressFunction = nullptr);
    extern MIFSA_EXPORT std::string getFileSha1(const std::string& filePath, const std::function<bool()>& breakFunction = nullptr, const std::function<void(size_t, size_t)>& progressFunction = nullptr);
    extern MIFSA_EXPORT std::string getFileSha256(const std::string& filePath, const std::function<bool()>& breakFunction = nullptr, const std::function<void(size_t, size_t)>& progressFunction = nullptr);
}
}
MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_UTILS_DECRYPT_H
