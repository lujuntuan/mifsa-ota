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

#ifndef MIFSA_OTA_TYPES_DOMAIN_H
#define MIFSA_OTA_TYPES_DOMAIN_H

#include "mifsa/base/elapsed.h"
#include "transfer.h"
#include "upgrade.h"
#include <functional>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Ota {

enum ServerState {
    MR_UNKNOWN = 0,
    MR_OFFLINE,
    MR_IDLE,
    MR_PENDING,
    MR_READY,
    MR_DOWNLOAD,
    MR_VERIFY,
    MR_DISTRIBUTE,
    MR_WAIT,
    MR_DEPLOY,
    MR_CANCEL,
    MR_DOWNLOAD_ASK,
    MR_DEPLOY_ASK,
    MR_CANCEL_ASK,
    MR_RESUME_ASK,
    MR_DONE_ASK,
    MR_ERROR_ASK,
};
enum ClientState {
    WR_UNKNOWN = 0,
    WR_OFFLINE,
    WR_ERROR,
    WR_IDLE,
    WR_DOWNLOAD,
    WR_VERIFY,
    WR_PATCH,
    WR_WAIT,
    WR_DEPLOY,
    WR_CANCEL,
};
enum Control {
    CTL_UNKNOWN = 0,
    CTL_RESET,
    CTL_DOWNLOAD,
    CTL_DEPLOY,
    CTL_CANCEL,
    CTL_CLEAR,
};
enum Answer {
    ANS_UNKNOWN = 0,
    ANS_ACCEPT,
    ANS_REFUSE,
    ANS_OTHER,
};
enum Discovery {
    DSC_NONE = 0,
    DSC_CONTROL,
    DSC_DETAIL,
};

using Depends = std::vector<std::string>;

struct MIFSA_EXPORT Domain final {
    explicit Domain(const std::string& _name = "", const std::string& _guid = "") noexcept
        : name(_name)
        , guid(_guid)
    {
    }
    std::string name;
    std::string guid;
    ClientState state = WR_UNKNOWN;
    ClientState last = WR_UNKNOWN;
    bool watcher = false;
    int error = 0;
    std::string version;
    VariantMap attribute;
    VariantMap meta;
    float progress = .0f;
    std::string message;
    Answer answer = ANS_UNKNOWN;

public:
    static std::string getMrStateStr(ServerState state) noexcept;
    static std::string getWrStateStr(ClientState state) noexcept;
    static std::string getControlStr(Control control) noexcept;
    static std::string getAnswerStr(Answer answer) noexcept;
    static bool mrStateIsBusy(ServerState state) noexcept;
    static bool wrStateIsBusy(ClientState state) noexcept;
    static bool mrStateIsAsk(ServerState state) noexcept;
    void update(const Domain& domain) noexcept;
    bool isEqual(const Domain& domain) const noexcept;
    bool operator==(const Domain& domain) const noexcept;
    bool operator!=(const Domain& domain) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Domain& domain) noexcept;
};

struct MIFSA_EXPORT Detail final {
    explicit Detail(const Domain& _domain = Domain()) noexcept
        : domain(_domain)
    {
    }
    explicit Detail(Domain&& _domain) noexcept
        : domain(std::move(_domain))
    {
    }
    Domain domain;
    Package package;
    Transfers transfers;
    float progress = .0f;
    Elapsed deploy;
    Elapsed heartbeat;

public:
    bool detectVersionEqual() const noexcept;
    bool detectVersionVaild() const noexcept;
    bool hasDepends(const Depends& depends) const noexcept;
    bool operator==(const Detail& detail) const noexcept;
    bool operator!=(const Detail& detail) const noexcept;
    bool operator<(const Detail& detail) const noexcept;
    bool operator>(const Detail& detail) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Detail& detail) noexcept;
};

class MIFSA_EXPORT Details final : public std::vector<Detail> {

public:
    Detail* update(Domain&& domain, bool force = false) noexcept;
    void sort() noexcept;
    Detail* find(const std::string& name) noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Details& details) noexcept;
};
}
VARIANT_DECLARE_TYPE(Ota::Domain, ota_domain);
VARIANT_DECLARE_TYPE(Ota::Details, ota_details);

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_TYPES_DOMAIN_H
