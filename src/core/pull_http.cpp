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

#include "mifsa/ota/setting.h"
#if (defined(MIFSA_SERVER_TYPE) && defined(MIFSA_USE_PULL_HTTP))
#include "core.h"

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Core {
    extern Status httpDownloadCommon(const std::string& dir, const Files& files, const VariantMap& config,
        const BreakFunction& breakFunction,
        const ProgressFunction& progressFunction);

    Status pull(const std::string& dir, const Files& files, const VariantMap& config,
        const BreakFunction& breakFunction,
        const ProgressFunction& progressFunction)
    {
        return httpDownloadCommon(dir, files, config, breakFunction, progressFunction);
    }
}
}

MIFSA_NAMESPACE_END

#endif
