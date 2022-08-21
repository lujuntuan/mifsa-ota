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

#ifndef MIFSA_OTA_UPGRADE_H
#define MIFSA_OTA_UPGRADE_H

#include "mifsa/base/define.h"
#include "mifsa/base/variant.h"
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct File;
struct Package;
using Files = std::vector<File>;
using Packages = std::vector<Package>;

struct MIFSA_EXPORT File final {
    std::string domain;
    std::string name;
    std::string url;
    std::string md5;
    std::string sha1;
    std::string sha256;
    uint64_t size = 0;
    std::string web_url;

public:
    static std::string getSizeStr(uint32_t size) noexcept;
    bool operator==(const File& file) const noexcept;
    bool operator!=(const File& file) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const File& file) noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Files& files) noexcept;
};

struct MIFSA_EXPORT Package final {
    std::string domain;
    std::string part;
    std::string version;
    VariantMap meta;
    Files files;

public:
    bool operator==(const Package& package) const noexcept;
    bool operator!=(const Package& package) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Package& package) noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Packages& packages) noexcept;
};

struct MIFSA_EXPORT Upgrade final {
    enum Method {
        MTHD_SKIP = 0,
        MTHD_ATTEMPT,
        MTHD_FORCED
    };
    std::string id;
    Method download = MTHD_SKIP;
    Method deploy = MTHD_SKIP;
    bool maintenance = false;
    Packages packages;

public:
    static std::string getMethodStr(int method) noexcept;
    bool operator==(const Upgrade& upgrade) const noexcept;
    bool operator!=(const Upgrade& upgrade) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Upgrade& upgrade) noexcept;
};
}
VARIANT_DECLARE_TYPE(Ota::File, ota_file);
VARIANT_DECLARE_TYPE(Ota::Package, ota_package);
VARIANT_DECLARE_TYPE(Ota::Upgrade, ota_upgrade);

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_UPGRADE_H
