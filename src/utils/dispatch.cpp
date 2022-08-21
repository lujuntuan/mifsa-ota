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

#include "dispatch.h"
#include "mifsa/utils/string.h"
#include <cstring>
#include <fstream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Utils {
    bool dispatch(const std::string& oldFile, const std::string& newFile, const std::string& patchFile,
        const std::function<bool()>& breakFunction, const std::function<void(size_t, size_t)>& progressFunction)
    {
        MIFSA_UNUSED(breakFunction);
        MIFSA_UNUSED(progressFunction);
        int reval = system(Mifsa::Utils::stringSprintf(R"(bspath %s %s %s)", oldFile, newFile, patchFile).c_str());
        if (reval != 0) {
            return false;
        }
        return true;
    }
}
}

MIFSA_NAMESPACE_END
