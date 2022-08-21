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
#if (defined(MIFSA_CLIENT_TYPE) && defined(MIFSA_USE_PATCH_BSDIFF))
#include "core.h"
#include "helper.h"
#include "importlib/httplib.hpp"
#include "utils/dispatch.h"
#include <mifsa/base/log.h>
#include <mifsa/utils/dir.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Core {
    Status patch(const std::string& dir, const std::vector<std::string>& paths, Files& newFiles,
        const BreakFunction& breakFunction,
        const ProgressFunction& progressFunction)
    {
        MIFSA_UNUSED(progressFunction);
        Files targetNewFiles;
        StatusHelper statusHelper;
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        httplib::ThreadPool threadPool(paths.size() > CPPHTTPLIB_THREAD_POOL_COUNT ? CPPHTTPLIB_THREAD_POOL_COUNT : paths.size());
        for (const auto& path : paths) {
            threadPool.enqueue([&]() {
                const std::string& infoPath = path + ".info";
                const std::string& newPath = path.substr(0, path.length() - 6);
                if (!Mifsa::Utils::exists(path)) {
                    statusHelper.throwError(701);
                    return;
                }
                if (Mifsa::Utils::getPathSuffixName(path) != "patch") {
                    statusHelper.throwError(702);
                    return;
                }
                if (!Mifsa::Utils::exists(infoPath)) {
                    statusHelper.throwError(703);
                    return;
                }
                const VariantMap& info = Variant::readJson(infoPath);
                const std::string& oldName = info.value("basename").toString();
                if (oldName.empty()) {
                    statusHelper.throwError(704);
                    return;
                }
                const std::string& sha256 = info.value("sha256").toString();
                const std::string& sha1 = info.value("sha1").toString();
                const std::string& md5 = info.value("md5").toString();
                std::string oldPath;
                for (const auto& p : Mifsa::Utils::getAllFiles(dir)) {
                    if (Mifsa::Utils::getPathFileName(p) == oldName) {
                        oldPath = p;
                        break;
                    }
                }
                if (oldPath.empty()) {
                    statusHelper.throwError(705);
                    return;
                }
                bool dispathOk = Utils::dispatch(oldPath, newPath, path, breakFunction);
                if (statusHelper.checkDone()) {
                    return;
                }
                if (!dispathOk) {
                    statusHelper.throwError(706);
                    return;
                }
                File file;
                file.name = Mifsa::Utils::getPathFileName(path);
                file.sha256 = sha256;
                file.sha1 = sha1;
                file.md5 = md5;
                targetNewFiles.push_back(file);
            });
        }
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        threadPool.shutdown();
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        newFiles = targetNewFiles;
        return statusHelper.status;
    }
}
}

MIFSA_NAMESPACE_END

#endif
