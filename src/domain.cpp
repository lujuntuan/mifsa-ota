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

#include "mifsa/ota/domain.h"
#include "mifsa/utils/string.h"
#include <algorithm>
#include <iostream>

MIFSA_NAMESPACE_BEGIN

namespace Ota {

std::string Domain::getMrStateStr(ServerState state) noexcept
{
    switch (state) {
    case MR_UNKNOWN:
        return "UNKNOWN";
    case MR_OFFLINE:
        return "OFFLINE";
    case MR_PENDING:
        return "PENDING";
    case MR_IDLE:
        return "IDLE";
    case MR_READY:
        return "READY";
    case MR_DOWNLOAD:
        return "DOWNLOAD";
    case MR_VERIFY:
        return "VERIFY";
    case MR_DISTRIBUTE:
        return "DISTRIBUTE";
    case MR_WAIT:
        return "MR_WAIT";
    case MR_DEPLOY:
        return "DEPLOY";
    case MR_CANCEL:
        return "CANCEL";
    case MR_DOWNLOAD_ASK:
        return "DOWNLOAD_ASK";
    case MR_DEPLOY_ASK:
        return "DEPLOY_ASK";
    case MR_CANCEL_ASK:
        return "CANCEL_ASK";
    case MR_RESUME_ASK:
        return "MR_RESUME_ASK";
    case MR_DONE_ASK:
        return "DONE_ASK";
    case MR_ERROR_ASK:
        return "ERROR_ASK";
    default:
        return "UNKNOWN";
    }
}

std::string Domain::getWrStateStr(ClientState state) noexcept
{
    switch (state) {
    case WR_UNKNOWN:
        return "UNKNOWN";
    case WR_OFFLINE:
        return "OFFLINE";
    case WR_ERROR:
        return "ERROR";
    case WR_IDLE:
        return "IDLE";
    case WR_DOWNLOAD:
        return "DOWNLOAD";
    case WR_VERIFY:
        return "VERIFY";
    case WR_PATCH:
        return "PATCH";
    case WR_WAIT:
        return "WAIT";
    case WR_DEPLOY:
        return "DEPLOY";
    case WR_CANCEL:
        return "CANCEL";
    default:
        return "UNKNOWN";
    }
}

std::string Domain::getControlStr(Control control) noexcept
{
    switch (control) {
    case CTL_UNKNOWN:
        return "UNKNOWN";
    case CTL_RESET:
        return "RESET";
    case CTL_DOWNLOAD:
        return "DOWNLOAD";
    case CTL_DEPLOY:
        return "DEPLOY";
    case CTL_CLEAR:
        return "CLEAR";
    default:
        return "UNKNOWN";
    }
}

std::string Domain::getAnswerStr(Answer answer) noexcept
{
    switch (answer) {
    case ANS_UNKNOWN:
        return "UNKNOWN";
    case ANS_ACCEPT:
        return "ACCEPT";
    case ANS_REFUSE:
        return "REFUSE";
    default:
        return "UNKNOWN";
    }
}

bool Domain::mrStateIsBusy(ServerState state) noexcept
{
    if (state == MR_PENDING || state == MR_READY
        || state == MR_DOWNLOAD || state == MR_VERIFY || state == MR_DISTRIBUTE
        || state == MR_WAIT || state == MR_DEPLOY || state == MR_CANCEL) {
        return true;
    }
    return false;
}

bool Domain::wrStateIsBusy(ClientState state) noexcept
{
    if (state == WR_DOWNLOAD || state == WR_VERIFY || state == WR_PATCH
        || state == WR_WAIT || state == WR_DEPLOY || state == WR_CANCEL) {
        return true;
    }
    return false;
}

bool Domain::mrStateIsAsk(ServerState state) noexcept
{
    if (state == MR_DOWNLOAD_ASK || state == MR_DEPLOY_ASK || state == MR_CANCEL_ASK || state == MR_RESUME_ASK
        || state == MR_DONE_ASK || state == MR_ERROR_ASK) {
        return true;
    }
    return false;
}

void Domain::update(const Domain& domain) noexcept
{
    if (name != domain.name) {
        return;
    }
    state = domain.state;
    last = domain.last;
    watcher = domain.watcher;
    error = domain.error;
    version = domain.version;
    attribute = domain.attribute;
    meta = domain.meta;
    progress = domain.progress;
    message = domain.message;
    answer = domain.answer;
}

bool Domain::isEqual(const Domain& domain) const noexcept
{
    return name == domain.name
        //&&guid == domain.guid
        && state == domain.state
        && last == domain.last
        && watcher == domain.watcher
        && error == domain.error
        && version == domain.version
        && attribute == domain.attribute
        && meta == domain.meta
        && progress == domain.progress
        && message == domain.message
        && answer == domain.answer;
}

bool Domain::operator==(const Domain& domain) const noexcept
{
    return isEqual(domain);
}

bool Domain::operator!=(const Domain& domain) const noexcept
{
    return !isEqual(domain);
}

std::ostream& operator<<(std::ostream& ostream, const Domain& domain) noexcept
{
    if (domain.name.empty()) {
        ostream << "{\n  Unavailable domain\n}";
        return ostream;
    }
    ostream << "{\n";
    ostream << "  [name]: " << domain.name << "\n";
    if (!domain.guid.empty()) {
        ostream << "  [guid]: " << domain.guid << "\n";
    }
    ostream << "  [state]: " << Domain::getWrStateStr(domain.state) << "\n";
    ostream << "  [last]: " << Domain::getWrStateStr(domain.last) << "\n";
    if (domain.watcher) {
        ostream << "  [watcher]: " << std::string("true") << "\n";
    }
    if (domain.error != 0) {
        ostream << "  [error]: " << domain.error << "\n";
    }
    if (!domain.version.empty()) {
        ostream << "  [version]: " << domain.version << "\n";
    }
    if (!domain.attribute.empty()) {
        ostream << "  [attribute]: " << domain.attribute.toJson() << "\n";
    }
    if (!domain.meta.empty()) {
        ostream << "  [meta]: " << domain.meta.toJson() << "\n";
    }
    if (domain.progress != 0) {
        ostream << "  [progress]: " << Utils::doubleToString(domain.progress) << "\n";
    }
    if (!domain.message.empty()) {
        ostream << "  [message]: " << domain.message << "\n";
    }
    if (domain.answer != ANS_UNKNOWN) {
        ostream << "  [answer]: " << Domain::getAnswerStr(domain.answer) << "\n";
    }
    ostream << "}";
    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const Depends& depends) noexcept
{
    int i = 0;
    for (const auto& s : depends) {
        if (i > 0) {
            ostream << ", ";
        }
        ostream << s;
        i++;
    }
    return ostream;
}

bool Detail::detectVersionEqual() const noexcept
{
    if (domain.version.empty()) {
        return false;
    }
    return domain.version == package.version;
}

bool Detail::detectVersionVaild() const noexcept
{
    if (domain.version.empty() || package.version.empty()) {
        return false;
    }
    const auto currentVersionList = Utils::stringSplit(domain.version, ".");
    const auto targetVersionList = Utils::stringSplit(package.version, ".");
    if (currentVersionList.size() != targetVersionList.size()) {
        return false;
    }
    for (unsigned i = 0; i < currentVersionList.size(); i++) {
        int selfNum = -1;
        try {
            selfNum = std::stoi(currentVersionList[i]);
        } catch (...) {
            return false;
        }
        int targetNum = -1;
        try {
            targetNum = std::stoi(targetVersionList[i]);
        } catch (...) {
            return false;
        }
        if (targetNum < -1) {
            return false;
        }
        if (targetNum > selfNum) {
            return true;
        }
    }
    return false;
}

inline static std::vector<std::string> getDependsNames(const VariantMap& meta)
{
    std::vector<std::string> depends;
    std::string dependsStr = meta.value("depends").toString();
    if (dependsStr.empty()) {
        return depends;
    }
    Utils::stringReplace(dependsStr, "\n", " ");
    depends = Utils::stringSplit(dependsStr, " ");
    return depends;
}

bool Detail::hasDepends(const Depends& depends) const noexcept
{
    const auto& requireNames = getDependsNames(package.meta);
    for (const auto& str : requireNames) {
        if (str.empty()) {
            continue;
        }
        for (const auto& name : depends) {
            if (name == str) {
                return true;
            }
        }
    }
    return false;
}

bool Detail::operator==(const Detail& detail) const noexcept
{
    return domain == detail.domain;
}

bool Detail::operator!=(const Detail& detail) const noexcept
{
    return domain != detail.domain;
}

bool Detail::operator<(const Detail& detail) const noexcept
{
    if (domain == detail.domain) {
        return false;
    }
    const auto& requireNames = getDependsNames(detail.package.meta);
    for (const auto& str : requireNames) {
        if (str.empty()) {
            continue;
        }
        if (str == domain.name) {
            return true;
        }
    }
    return domain.name < detail.domain.name;
}

bool Detail::operator>(const Detail& detail) const noexcept
{
    if (domain == detail.domain) {
        return true;
    }
    return !(*this < detail);
}

std::ostream& operator<<(std::ostream& ostream, const Detail& detail) noexcept
{
    ostream << detail.domain << "\n";
    if (!detail.package.files.empty()) {
        ostream << "  [package-size]: " << detail.package.files.size() << "\n";
    }
    if (!detail.transfers.empty()) {
        ostream << "  [transfers-size]: " << detail.transfers.size() << "\n";
    }
    if (detail.progress != 0) {
        ostream << "  [progress]: " << Utils::doubleToString(detail.progress) << "\n";
    }
    if (detail.deploy.active()) {
        ostream << "  [deploy]: " << detail.deploy.get() << std::string(" ms") << "\n";
    }
    if (detail.heartbeat.active()) {
        ostream << "  [heartbeat]: " << detail.heartbeat.get() << std::string(" ms") << "\n";
    }
    return ostream;
}

Detail* Details::update(Domain&& domain, bool force) noexcept
{
    for (Detail& d : *this) {
        if (d.domain.name == domain.name) {
            if (force) {
                d.domain = domain;
            } else {
                d.domain.update(domain);
            }
            return &d;
        }
    }
    if (force) {
        push_back(Detail(std::move(domain)));
        return &(this->back());
    }
    return nullptr;
}

void Details::sort() noexcept
{
    std::sort(begin(), end(), [](const Detail& lhs, const Detail& rhs) {
        return lhs < rhs;
    });
}

Detail* Details::find(const std::string& name) noexcept
{
    for (auto& d : *this) {
        if (d.domain.name == name) {
            return &d;
        }
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& ostream, const Details& domains) noexcept
{
    ostream << "[\n";
    for (const auto& d : domains) {
        ostream << d << "\n";
    }
    ostream << "]";
    return ostream;
}

}
MIFSA_NAMESPACE_END
