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

#include "setting.h"
#if (defined(MIFSA_OTA_ENABLE_VERIFY_OPENSSL))
#include "core.h"
#include "helper.h"
#include "hpplib/httplib.hpp"
#include "utils/verify.h"
#include <list>
#include <mifsa/base/elapsed.h>
#include <mifsa/base/log.h>
#include <mifsa/utils/dir.h>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
namespace Core {
    Status verify(const std::string& dir, const Files& files,
        const BreakFunction& breakFunction,
        const ProgressFunction& progressFunction)
    {
        StatusHelper statusHelper(breakFunction);
        if (files.empty()) {
            statusHelper.throwError(201);
            LOG_WARNING("verify files is empty");
            return statusHelper.status;
        }
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        if (!Mifsa::Utils::exists(dir)) {
            Mifsa::Utils::mkPath(dir);
        }
        std::list<std::string> verifyUrls;
        Transfers transfers;
        std::mutex mutex;
        Elapsed verifyElapsed;
        verifyElapsed.start();
        for (const File& file : files) {
            verifyUrls.push_back(file.url);
        }
        auto dealProgress = [&](const File& file, Elapsed& elapsed, uint64_t total, uint64_t current) {
            if (!progressFunction) {
                return;
            }
            if (elapsed.get() >= MIFSA_OTA_WEB_TRANSFER_INTERVAL_MIN || current >= total) {
                elapsed.restart();
                uint32_t pass = (uint32_t)(verifyElapsed.get() / 1000);
                uint32_t speed = pass <= 0 ? (uint32_t)(total / 1024) : (uint32_t)(current / 1024 / pass);
                if (speed > (uint32_t)(total / 1024)) {
                    speed = (uint32_t)(total / 1024);
                }
                uint32_t left = speed <= 0 ? 0 : (uint32_t)((total - current) / 1024 / speed);
                Transfer transfer;
                transfer.domain = file.domain;
                transfer.name = file.name;
                transfer.progress = current * 100.0f / total;
                transfer.total = (uint32_t)(total / 1024);
                transfer.current = (uint32_t)(current / 1024);
                transfer.speed = speed;
                transfer.pass = pass;
                transfer.left = left;
                mutex.lock();
                transfers.update(std::move(transfer), true);
                mutex.unlock();
                if (verifyElapsed.get() > MIFSA_OTA_WEB_TRANSFER_INTERVAL && current < total) {
                    mutex.lock();
                    verifyElapsed.restart();
                    transfers.sort();
                    progressFunction(transfers);
                    mutex.unlock();
                }
            }
        };
        httplib::ThreadPool threadPool(files.size() > CPPHTTPLIB_THREAD_POOL_COUNT ? CPPHTTPLIB_THREAD_POOL_COUNT : files.size());
        for (const auto& file : files) {
            threadPool.enqueue([&, file]() {
                if (statusHelper.checkDone()) {
                    return;
                }
                std::string filePath = dir + "/" + file.domain + "/" + file.name;
                Elapsed elapsed;
                elapsed.start();
                if (!Mifsa::Utils::exists(filePath)) {
                    statusHelper.throwError(202);
                    LOG_WARNING("file not exists", " (" + filePath + ")");
                    return;
                }
                if (!file.sha256.empty()) {
                    std::string targetSha256 = Utils::getFileSha256(filePath,
                        std::bind(&StatusHelper::checkDone, &statusHelper),
                        std::bind(dealProgress, file, elapsed, std::placeholders::_1, std::placeholders::_2));
                    if (statusHelper.checkDone()) {
                        return;
                    }
                    if (targetSha256.empty()) {
                        statusHelper.throwError(207);
                        LOG_WARNING("read sha256 error", " (" + filePath + ")");
                        return;
                    }
                    if (targetSha256 != file.sha256) {
                        statusHelper.throwError(208);
                        LOG_WARNING("check sha256 error", " (" + filePath + ")");
                        return;
                    }
                } else if (!file.sha1.empty()) {
                    std::string targetSha1 = Utils::getFileSha1(filePath,
                        std::bind(&StatusHelper::checkDone, &statusHelper),
                        std::bind(dealProgress, file, elapsed, std::placeholders::_1, std::placeholders::_2));
                    if (statusHelper.checkDone()) {
                        return;
                    }
                    if (targetSha1.empty()) {
                        statusHelper.throwError(205);
                        LOG_WARNING("read sha1 error", " (" + filePath + ")");
                        return;
                    }
                    if (targetSha1 != file.sha1) {
                        statusHelper.throwError(206);
                        LOG_WARNING("check sha1 error", " (" + filePath + ")");
                        return;
                    }
                } else if (!file.md5.empty()) {
                    std::string targetMd5 = Utils::getFileMd5(filePath,
                        std::bind(&StatusHelper::checkDone, &statusHelper),
                        std::bind(dealProgress, file, elapsed, std::placeholders::_1, std::placeholders::_2));
                    if (statusHelper.checkDone()) {
                        return;
                    }
                    if (targetMd5.empty()) {
                        statusHelper.throwError(203);
                        LOG_WARNING("read md5 error", " (" + filePath + ")");
                        return;
                    }
                    if (targetMd5 != file.md5) {
                        statusHelper.throwError(204);
                        LOG_WARNING("check md5 error", " (" + filePath + ")");
                        return;
                    }
                } else {
                    statusHelper.throwError(209);
                    LOG_WARNING("no verification", " (" + filePath + ")");
                    return;
                }
                mutex.lock();
                verifyUrls.remove(file.url);
                mutex.unlock();
            });
        }
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        threadPool.shutdown();
        if (statusHelper.checkDone()) {
            return statusHelper.status;
        }
        if (!verifyUrls.empty()) {
            statusHelper.throwError(210);
            LOG_WARNING("Verify final error");
        } else {
            if (progressFunction) {
                progressFunction(transfers);
            }
        }
        return statusHelper.status;
    }
}
}

MIFSA_NAMESPACE_END

#endif
