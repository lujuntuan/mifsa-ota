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

#ifndef MIFSA_OTA_WEB_FEED_H
#define MIFSA_OTA_WEB_FEED_H

#include <mifsa/base/define.h>
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct MIFSA_EXPORT WebFeed final {
    using Progress = std::pair<int, int>;
    using Details = std::vector<std::string>;
    enum Type {
        TP_UNKNOWN = 0,
        TP_DEPLOY,
        TP_CANCEL,
    };
    enum Execution {
        EXE_UNKNOWN = 0,
        EXE_CLOSED,
        EXE_PROCEEDING,
        EXE_CANCELED,
        EXE_SCHEDULED,
        EXE_REJECTED,
        EXE_RESUMED,
    };
    enum Result {
        RS_UNKNOWN = 0,
        RS_SUCCESS,
        RS_FAILURE,
        RS_NONE,
    };
    WebFeed() = default;
    explicit WebFeed(const std::string& _id, Type _type, Execution _execution, Result _result, const Details& _details = Details(), const Progress& _progress = { 1, 1 }) noexcept
        : id(_id)
        , type(_type)
        , execution(_execution)
        , result(_result)
        , details(_details)
        , progress(_progress)
    {
    }
    std::string id;
    Type type = TP_UNKNOWN;
    Execution execution = EXE_UNKNOWN;
    Result result = RS_UNKNOWN;
    Details details;
    Progress progress = std::make_pair(0, 0);

public:
    static std::string getTypeStr(int type) noexcept;
    static std::string getExecutionStr(int execution) noexcept;
    static std::string getResultStr(int result) noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const WebFeed& webStatus) noexcept;
};
}

MIFSA_NAMESPACE_END

#endif // MIFSA_OTA_WEB_FEED_H
