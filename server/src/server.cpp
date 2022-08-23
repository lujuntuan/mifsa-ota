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

#include "server.h"
#include "adapter/adapter.h"
#include "hawkbit_queue.h"
#include "server_event.h"
#include "web_event.h"
#include "web_init.h"
#include <mifsa/utils/dir.h>
#include <mifsa/utils/string.h>
#include <mifsa/utils/system.h>

#define m_hpr m_serverHelper

int main(int argc, char* argv[])
{
    Mifsa::Ota::Server server(argc, argv);
    return server.exec();
}

MIFSA_NAMESPACE_BEGIN

namespace Ota {
struct ServerHelper {
    bool active = true;
    bool firstStart = false;
    bool hasFeed = false;
    bool detailSubscribed = false;
    bool hasDeployBreaked = false;
    bool hasNewId = false;
    bool cancelResume = false;
    ServerState state = MR_OFFLINE;
    ServerState lastState = MR_OFFLINE;
    ServerState cacheState = MR_UNKNOWN;
    ServerState cacheLastState = MR_UNKNOWN;
    Control control = CTL_UNKNOWN;
    Control lastControl = CTL_UNKNOWN;
    int retryTimes = 0;
    int errorCode = 0;
    uint32_t controlMessageId = 0;
    uint32_t detailMessageId = 0;
    float step = .0f;
    float progress = .0f;
    std::string message;
    std::string statusFilePath;
    std::string cancelId;
    std::string lastlId;
    std::string lastCancelId;
    std::shared_ptr<Timer> processDomainsTimer;
    VariantMap attributes;
    VariantMap domainsConfig;
    VariantMap cacheStatus;
    Depends cacheDepends;
    HawkbitQueue webQueue;
    Upgrade upgrade;
    Details details;
    Depends depends;
    Elapsed stateElapsed;
    WebFeed webFeed;
    const Application::Arg argVersion { "v", "version", " module version" };
    const Application::Arg argUrl { "u", "url", " server url", "http://localhost:8080" };
    const Application::Arg argTenant { "t", "tenant", " tenant name", "DEFAULT" };
    const Application::Arg argId { "i", "id", " id name", "123456789" };
    const Application::Arg argToken { "k", "token", " token number", "" };
};

Server::Server(int argc, char** argv)
    : ServerProxy(argc, argv, "ota", MIFSA_OTA_QUEUE_ID_SERVER)
{
    setInstance(this);
    MIFSA_HELPER_CREATE(m_hpr);
    //
    parserArgs({ m_hpr->argVersion, m_hpr->argUrl, m_hpr->argTenant, m_hpr->argId, m_hpr->argToken });
    if (getArgValue(m_hpr->argVersion).toBool()) {
        LOG_DEBUG(MIFSA_OTA_VERSION);
        std::exit(0);
        return;
    }
    static std::mutex mutex;
    setMutex(mutex);
    m_hpr->webQueue.setMutex(mutex);
    if (m_hpr->webQueue.isRunning()) {
        LOG_WARNING("web queue is running");
        return;
    }
    m_hpr->domainsConfig = readConfig("mifsa_ota_domains.json");
    Variant webUrl = getArgValue(m_hpr->argUrl, "web_url");
    Variant tenant = getArgValue(m_hpr->argTenant, "tenant");
    Variant id = getArgValue(m_hpr->argId, "id");
    Variant token = getArgValue(m_hpr->argToken, "token");
    WebInit webInit(webUrl.toString(), tenant.toString(), id.toString(), { "GatewayToken", token.toString() }); // GatewayToken or TargetToken
    m_hpr->webQueue.postEvent(std::make_shared<WebInitEvent>(webInit));
    m_hpr->webQueue.asyncRun();
    //
    loadInterface<ServerInterfaceAdapter>();
    interface()->setCbReportDomain([this](const DomainMessage& domainMessage) {
        processDomainMessage(domainMessage);
    });
}

Server::~Server()
{
    m_hpr->webQueue.quit();
    MIFSA_HELPER_DESTROY(m_hpr);
    setInstance(nullptr);
}

const Upgrade& Server::upgrade() const
{
    return m_hpr->upgrade;
}

const Details& Server::details() const
{
    return m_hpr->details;
}

const Depends& Server::depends() const
{
    return m_hpr->depends;
}

const std::string& Server::cancelId() const
{
    return m_hpr->cancelId;
}

const std::string& Server::message() const
{
    return m_hpr->message;
}

const VariantMap Server::attributes() const
{
    return m_hpr->attributes;
}

ServerState Server::state() const
{
    return m_hpr->state;
}

ServerState Server::lastState() const
{
    return m_hpr->lastState;
}

Control Server::control() const
{
    return m_hpr->control;
}

int Server::errorCode() const
{
    return m_hpr->errorCode;
}

bool Server::isActive() const
{
    return m_hpr->active;
}

float Server::step() const
{
    return m_hpr->step;
}

float Server::progress() const
{
    return m_hpr->progress;
}

void Server::processDomainMessage(const DomainMessage& domainMessage)
{
    if (!m_hpr->domainsConfig.empty()) {
        if (m_hpr->domainsConfig.contains(domainMessage.domain.name)) {
            if (domainMessage.domain.guid != m_hpr->domainsConfig.value(domainMessage.domain.name).toString()) {
                LOG_WARNING("guid verification failed");
                return;
            }
        } else {
            LOG_WARNING("guid search failed");
            return;
        }
    }
    this->postEvent(std::make_shared<ServerDomainEvent>(std::move(domainMessage.domain), domainMessage.discovery));
}

void Server::begin()
{
    ServerProxy::begin();
    std::string cacheDir = Utils::getTempDir();
    if (config().value("cache_dir").isValid()) {
        cacheDir = config().value("cache_dir").toString();
    }
    if (!Utils::exists(cacheDir)) {
        Utils::mkPath(cacheDir);
    }
    m_hpr->statusFilePath = cacheDir + "/mifsa_ota_status.json";
    if (!Utils::exists(m_hpr->statusFilePath)) {
        m_hpr->firstStart = true;
    }
    //
    m_hpr->processDomainsTimer = createTimer(MIFSA_OTA_PROCESS_DOMAIN_TIME, true, std::bind(&Server::processDomains, this));
    m_hpr->processDomainsTimer->start();
}

void Server::end()
{
    ServerProxy::begin();
}

void Server::eventChanged(const std::shared_ptr<Event>& event)
{
    auto serverEvent = std::static_pointer_cast<ServerEvent>(event);
    if (serverEvent->type() == ServerEvent::FUNCTION) {
        return;
    }
    if (serverEvent->isAccepted()) {
        return;
    }
    if (serverEvent->type() == ServerEvent::RES_ERROR) {
        int error = serverEvent->data().value("error").toInt();
        if (m_hpr->state == MR_DOWNLOAD || m_hpr->state == MR_VERIFY || m_hpr->state == MR_DISTRIBUTE) {
            if (m_hpr->retryTimes < MIFSA_OTA_RETRY_TIMES) {
                m_hpr->webQueue.postEvent(std::make_shared<WebEvent>(WebEvent::REQ_CHECK));
                wait(1000);
                LOG_WARNING("retry download");
                download();
                m_hpr->retryTimes++;
            } else {
                m_hpr->retryTimes = 0;
                LOG_WARNING("retry times to much");
                feedback(false, 1000 + error);
            }
        } else {
            feedback(false, 1000 + error);
        }
        return;
    }
    switch (serverEvent->type()) {
    case ServerEvent::REQ_ACTIVE: {
        bool active = serverEvent->data().value("active").toBool();
        m_hpr->active = active;
        sendDetailMessage();
        break;
    }
    case ServerEvent::REQ_IDLE: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        idle();
        break;
    }
    case ServerEvent::REQ_UPGRADE: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        auto upgradeEvent = std::dynamic_pointer_cast<ServerUpgradeEvent>(serverEvent);
        if (!upgradeEvent) {
            LOG_WARNING("get ServerUpgradeEvent error");
            break;
        }
        m_hpr->hasFeed = false;
        m_hpr->webFeed.type = WebFeed::TP_DEPLOY;
        m_hpr->hasNewId = m_hpr->lastlId != upgradeEvent->upgrade().id;
        m_hpr->cancelResume = !m_hpr->hasNewId && !m_hpr->lastCancelId.empty() && m_hpr->lastCancelId == upgradeEvent->upgrade().id;
        lock();
        m_hpr->upgrade = upgradeEvent->upgrade();
        unlock();
        m_hpr->lastlId = m_hpr->upgrade.id;
        if (m_hpr->hasNewId) {
            m_hpr->hasDeployBreaked = false;
        }
        pending();
        LOG_DEBUG(upgrade());
        break;
    }
    case ServerEvent::REQ_CANCEL: {
        m_hpr->hasFeed = false;
        m_hpr->webFeed.type = WebFeed::TP_CANCEL;
        lock();
        m_hpr->cancelId = serverEvent->data().value("id").toString();
        unlock();
        m_hpr->lastCancelId = m_hpr->cancelId;
        if (m_hpr->state == MR_DEPLOY) {
            m_hpr->hasDeployBreaked = true;
        }
        if (m_hpr->state == MR_WAIT) {
            feedback(true);
            break;
        }
        if (Domain::mrStateIsAsk(m_hpr->state)) {
            feedback(true);
            break;
        }
        if (!Domain::mrStateIsBusy(m_hpr->state)) {
            feedback(false, 1900);
            WebFeed webFeed(m_hpr->cancelId, WebFeed::TP_DEPLOY, WebFeed::EXE_CLOSED, WebFeed::RS_FAILURE);
            m_hpr->webQueue.postEvent(std::make_shared<WebFeedEvent>(webFeed));
            break;
        }
        setState(MR_CANCEL_ASK);
        break;
    }
    case ServerEvent::REQ_PULL: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        if (m_hpr->webQueue.state() != WebQueue::WEB_DISTRIBUTE) {
            break;
        }
        if (m_hpr->upgrade.download != Upgrade::MTHD_SKIP) {
            wait(100);
            sendControlMessage(CTL_DOWNLOAD);
        }
        setState(MR_DISTRIBUTE);
        break;
    }
    case ServerEvent::RES_INIT: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        bool success = serverEvent->data().value("success").toBool();
        if (success) {
            readState();
            idle();
        }
        break;
    }
    case ServerEvent::RES_DOWNLOAD_DONE: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        for (auto& d : m_hpr->details) {
            d.transfers.clear();
            d.transfers.shrink_to_fit();
        }
        setState(MR_VERIFY);
        m_hpr->webQueue.postEvent(std::make_shared<WebEvent>(WebEvent::REQ_VERIFY));
        break;
    }
    case ServerEvent::RES_VERIFY_DONE: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        for (auto& d : m_hpr->details) {
            d.transfers.clear();
            d.transfers.shrink_to_fit();
        }
        setState(MR_DISTRIBUTE);
        m_hpr->webQueue.postEvent(std::make_shared<WebEvent>(WebEvent::REQ_DISTRIBUTE));
        break;
    }
    case ServerEvent::RES_DISTRUBUTE_DONE: {
        if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
            break;
        }
        for (auto& d : m_hpr->details) {
            d.transfers.clear();
            d.transfers.shrink_to_fit();
        }
        setState(MR_WAIT);
        break;
    }
    case ServerEvent::RES_FEEDBACK_DONE: {
        if (m_hpr->webFeed.type == WebFeed::TP_UNKNOWN) {
            LOG_WARNING("get RES_FEEDBACK_DONE error");
            break;
        }
        break;
    }
    case ServerEvent::RES_TRANSFER_PROGRESS: {
        auto transferEvent = std::dynamic_pointer_cast<ServerTransferEvent>(serverEvent);
        if (!transferEvent) {
            LOG_WARNING("get ServerTransferEvent error");
            break;
        }
        LOG_DEBUG(transferEvent->transfers());
        uint32_t all_current = 0;
        uint32_t all_total = 0;
        for (auto& d : m_hpr->details) {
            d.transfers.clear();
            d.transfers.shrink_to_fit();
            uint32_t current = 0;
            uint32_t total = 0;
            for (const auto& files : d.package.files) {
                total += (uint32_t)(files.size / 1024.0f);
            }
            for (const auto& transfer : transferEvent->transfers()) {
                if (transfer.domain == d.domain.name) {
                    current += transfer.current;
                    d.transfers.push_back(std::move(transfer));
                }
            }
            if (total > 0) {
                d.progress = current * 100.0f / total;
            }
            all_current += current;
            all_total += total;
        }
        if (all_total > 0) {
            m_hpr->step = all_current * 100.0f / all_total;
            if (m_hpr->webQueue.state() == WebQueue::WEB_DOWNLOAD) {
                m_hpr->progress = 10.0f + all_current * 20.0f / all_total;
            } else if (m_hpr->webQueue.state() == WebQueue::WEB_VERIFY) {
                m_hpr->progress = 30.0f + all_current * 5.0f / all_total;
            } else if (m_hpr->webQueue.state() == WebQueue::WEB_DISTRIBUTE) {
                m_hpr->progress = 35.0f + all_current * 15.0f / all_total;
            }
        }
        sendDetailMessage();
        break;
    }
    case ServerEvent::RES_DOMAIN: {
        auto serverDomainEvent = std::dynamic_pointer_cast<ServerDomainEvent>(serverEvent);
        if (!serverDomainEvent) {
            LOG_WARNING("get ServerControlReplyEvent error");
            break;
        }
        if (serverDomainEvent->domain().name.empty()) {
            LOG_WARNING("domain name is empty");
            break;
        }
        if (serverDomainEvent->domain().watcher) {
            m_hpr->detailSubscribed = true;
        }
        lock();
        for (const auto& pair : serverDomainEvent->domain().attribute) {
            m_hpr->attributes.insert(serverDomainEvent->domain().name + "_" + pair.first, pair.second);
        }
        m_hpr->attributes.insert(serverDomainEvent->domain().name + "_version", serverDomainEvent->domain().version);
        unlock();
        Detail* d = m_hpr->details.find(serverDomainEvent->domain().name);
        if (d) {
            d->heartbeat.restart();
            if (serverDomainEvent->discovery()) {
                sendControlMessage(m_hpr->control, true);
            }
            if (!d->domain.isEqual(serverDomainEvent->domain())) {
                d->domain.update(serverDomainEvent->domain());
                if (m_hpr->state == MR_DEPLOY && m_hpr->details.size() > 0) {
                    float all_progress = 0;
                    for (auto& td : m_hpr->details) {
                        if (td.domain.state == WR_IDLE || td.domain.state == WR_DEPLOY || (td.domain.state == WR_CANCEL && td.domain.last == WR_DEPLOY)) {
                            td.progress = td.domain.progress;
                            all_progress += td.domain.progress;
                        }
                    }
                    m_hpr->step = all_progress * 1.0f / m_hpr->details.size();
                    m_hpr->progress = 50.0f + all_progress / m_hpr->details.size() / 2;
                }
                sendDetailMessage();
            }
        } else {
            if (serverDomainEvent->domain().watcher && serverDomainEvent->discovery()) {
                sendControlMessage(m_hpr->control, true);
                sendDetailMessage(true);
            }
        }
        if (serverDomainEvent->domain().watcher && serverDomainEvent->domain().answer != ANS_UNKNOWN) {
            configAnswer(serverDomainEvent->domain().answer);
        }
        break;
    }
    default:
        break;
    }
}

std::vector<std::string> Server::getWebFeedDetails() const
{
    std::vector<std::string> details;
    details.push_back("Mifsa-OTA Message:");
    details.push_back(Utils::stringSprintf("[server] state: %s, last: %s, control: %s, error: %s, step: %s, progress: %s, message: %s",
        Domain::getMrStateStr(m_hpr->state),
        Domain::getMrStateStr(m_hpr->lastState),
        Domain::getControlStr(m_hpr->control),
        std::to_string(m_hpr->errorCode),
        Utils::doubleToString(m_hpr->step, 2),
        Utils::doubleToString(m_hpr->progress, 2),
        m_hpr->message));
    for (const auto& d : m_hpr->details) {
        details.push_back(Utils::stringSprintf("[%s] state: %s, last: %s, watcher: %s, error: %s, version: %s, progress: %s, message: %s",
            d.domain.name,
            Domain::getWrStateStr(d.domain.state),
            Domain::getWrStateStr(d.domain.last),
            d.domain.watcher ? std::string("true") : std::string("false"),
            std::to_string(d.domain.error),
            d.domain.version,
            Utils::doubleToString(d.domain.progress, 2),
            d.domain.message));
    }
    return details;
}

std::pair<int, int> Server::getWebFeedProgress() const
{
    int step = 0;
    for (const auto& d : m_hpr->details) {
        if (d.domain.state == WR_IDLE && d.detectVersionEqual()) {
            step++;
        }
    }
    return { step, (int)m_hpr->details.size() };
}

void Server::configState(bool isLastError) // pending %10,download 30%,distribute 50%,deploy 100%
{
    ServerState state = m_hpr->state;
    if (isLastError) {
        state = m_hpr->lastState;
    }
    switch (state) {
    case MR_UNKNOWN: {
        if (isLastError) {
            m_hpr->message = "Unknown Errored !";
        } else {
            LOG_WARNING("set unknown state");
            m_hpr->step = .0f;
            m_hpr->progress = .0f;
            m_hpr->message = "Unknown State !";
        }
        break;
    }
    case MR_OFFLINE: {
        if (isLastError) {
            m_hpr->message = "Unknown Errored !";
        } else {
            LOG_WARNING("set offline state");
            m_hpr->step = .0f;
            m_hpr->progress = .0f;
            m_hpr->message = "Unknown State !";
        }
        break;
    }
    case MR_IDLE: {
        if (isLastError) {
            m_hpr->message = "Unknown Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = .0f;
            m_hpr->message = "";
        }
        break;
    }
    case MR_PENDING: {
        if (isLastError) {
            m_hpr->message = "Search domain Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = .0f;
            m_hpr->message = "Searching domain ...";
        }
        break;
    }
    case MR_READY: {
        if (isLastError) {
            m_hpr->message = "Search domain Errored !";
        } else {
            m_hpr->step = 100.0f;
            m_hpr->progress = 10.0f;
            m_hpr->message = "Ready ...";
        }
        break;
    }
    case MR_DOWNLOAD: {
        if (isLastError) {
            m_hpr->message = "Download Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = 10.0f;
            m_hpr->message = "Downloading ...";
        }
        break;
    }
    case MR_VERIFY: {
        if (isLastError) {
            m_hpr->message = "Verify Errored !";
        } else {
            m_hpr->step = 50.0f;
            m_hpr->progress = 30.0f;
            m_hpr->message = "Verifying ...";
        }
        break;
    }
    case MR_DISTRIBUTE: {
        if (isLastError) {
            m_hpr->message = "Distribute Errored !";
        } else {
            m_hpr->step = 100.0f;
            m_hpr->progress = 30.0f;
            m_hpr->message = "Distributing ...";
        }
        break;
    }
    case MR_WAIT: {
        if (isLastError) {
            m_hpr->message = "Wait Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = 50.0f;
            m_hpr->message = "Waiting for deploy ...";
        }
        break;
    }
    case MR_DEPLOY: {
        if (isLastError) {
            m_hpr->message = "Deploy Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = 50.0f;
            m_hpr->message = "Deploying ...";
        }
        break;
    }
    case MR_CANCEL: {
        if (isLastError) {
            m_hpr->message = "Cancel Errored !";
        } else {
            m_hpr->message = "Cancel ...";
        }
        break;
    }
    case MR_DOWNLOAD_ASK: {
        if (isLastError) {
            m_hpr->message = "Wait for download confirmation Errored !";
        } else {
            m_hpr->message = "Waiting for download confirmation ...";
        }
        break;
    }
    case MR_DEPLOY_ASK: {
        if (isLastError) {
            m_hpr->message = "Wait for deploy confirmation Errored !";
        } else {
            m_hpr->message = "Waiting for deploy confirmation ...";
        }
        break;
    }
    case MR_CANCEL_ASK: {
        if (isLastError) {
            m_hpr->message = "Wait for cancel confirmation Errored !";
        } else {
            m_hpr->message = "Waiting for cancel confirmation ...";
        }
        break;
    }
    case MR_RESUME_ASK: {
        if (isLastError) {
            m_hpr->message = "Wait for resume confirmation Errored !";
        } else {
            m_hpr->message = "Waiting for resume confirmation ...";
        }
        break;
    }
    case MR_DONE_ASK: {
        if (isLastError) {
            m_hpr->message = "Unknown Errored !";
        } else {
            if (m_hpr->lastState == MR_CANCEL) {
                m_hpr->step = .0f;
                m_hpr->progress = .0f;
                m_hpr->message = "cancel successed !";
            } else {
                m_hpr->step = 100.0f;
                m_hpr->progress = 100.0f;
                m_hpr->message = "upgrade successed !";
            }
        }
        break;
    }
    case MR_ERROR_ASK: {
        if (isLastError) {
            LOG_WARNING("two errors");
            m_hpr->message = "Two Errored !";
        } else {
            m_hpr->step = .0f;
            m_hpr->progress = .0f;
            m_hpr->message = "Errored !";
            configState(true);
        }
        break;
    }
    default:
        break;
    }
}

void Server::configAnswer(Answer answer)
{
    if (m_hpr->state == MR_DOWNLOAD_ASK) {
        if (answer == ANS_ACCEPT) {
            download();
        } else if (answer == ANS_REFUSE) {
            LOG_WARNING("user refuse");
            feedback(false, -1);
        }
    } else if (m_hpr->state == MR_DEPLOY_ASK) {
        if (answer == ANS_ACCEPT) {
            deploy();
        } else if (answer == ANS_REFUSE) {
            LOG_WARNING("user refuse");
            feedback(false, -1);
        }
    } else if (m_hpr->state == MR_CANCEL_ASK) {
        if (answer == ANS_ACCEPT) {
            cancel();
        } else if (answer == ANS_REFUSE) {
            LOG_WARNING("user refuse");
            feedback(false, -1);
        }
    } else if (m_hpr->state == MR_RESUME_ASK) {
        if (answer == ANS_ACCEPT) {
            resume();
        } else if (answer == ANS_REFUSE) {
            LOG_WARNING("user refuse");
            if (m_hpr->hasNewId && m_hpr->cacheState == MR_DEPLOY) {
                pending();
            } else {
                idle();
            }
        }
        m_hpr->hasDeployBreaked = false;
    } else if (m_hpr->state == MR_DONE_ASK) {
        if (answer == ANS_ACCEPT) {
            idle();
        }
    } else if (m_hpr->state == MR_ERROR_ASK) {
        if (answer == ANS_ACCEPT) {
            idle();
        }
    }
}

void Server::setState(ServerState state)
{
    if (m_hpr->state != state) {
        m_hpr->lastState = m_hpr->state;
        m_hpr->state = state;
        m_hpr->cacheStatus.insert("state", m_hpr->state);
        m_hpr->cacheStatus.insert("last", m_hpr->lastState);
        m_hpr->cacheStatus.insert("depends", m_hpr->depends);
        m_hpr->cacheStatus.saveJson(m_hpr->statusFilePath);
        LOG_PROPERTY("State", Domain::getMrStateStr(state));
        m_hpr->stateElapsed.restart();
        configState();
        sendDetailMessage();
        if (Domain::mrStateIsBusy(m_hpr->state)) {
            if (m_hpr->webFeed.type == WebFeed::TP_DEPLOY) {
                WebFeed webFeed(m_hpr->upgrade.id, WebFeed::TP_DEPLOY, WebFeed::EXE_PROCEEDING, WebFeed::RS_SUCCESS, getWebFeedDetails(), getWebFeedProgress());
                m_hpr->webQueue.postEvent(std::make_shared<WebFeedEvent>(webFeed));
            } else {
                WebFeed webFeed(m_hpr->cancelId, WebFeed::TP_CANCEL, WebFeed::EXE_PROCEEDING, WebFeed::RS_SUCCESS, getWebFeedDetails(), getWebFeedProgress());
                m_hpr->webQueue.postEvent(std::make_shared<WebFeedEvent>(webFeed));
            }
        }
    }
}

void Server::readState()
{
    m_hpr->cacheStatus = Variant::readJson(m_hpr->statusFilePath);
    int state = m_hpr->cacheStatus.value("state").toInt();
    if (state >= 0) {
        m_hpr->cacheState = (ServerState)state;
    }
    int last = m_hpr->cacheStatus.value("last").toInt();
    if (last >= 0) {
        m_hpr->cacheLastState = (ServerState)last;
    }
    m_hpr->cacheDepends = m_hpr->cacheStatus.value("depends").toStringList();
}

void Server::resume()
{
    m_hpr->lastState = m_hpr->cacheLastState;
    m_hpr->depends = m_hpr->cacheDepends;
    deploy();
    m_hpr->cacheState = MR_UNKNOWN;
}

void Server::idle()
{
    if (m_hpr->state != MR_IDLE) {
        m_hpr->depends.clear();
        m_hpr->depends.shrink_to_fit();
        m_hpr->details.clear();
        m_hpr->details.shrink_to_fit();
        m_hpr->detailSubscribed = false;
        lock();
        m_hpr->upgrade = Upgrade();
        m_hpr->cancelId.clear();
        m_hpr->cancelId.shrink_to_fit();
        unlock();
        setState(MR_IDLE);
    }
}

void Server::pending()
{
    m_hpr->errorCode = 0;
    m_hpr->depends.clear();
    m_hpr->depends.shrink_to_fit();
    m_hpr->details.clear();
    m_hpr->details.shrink_to_fit();
    for (const auto& package : m_hpr->upgrade.packages) {
        Domain domain(package.domain);
        domain.state = WR_OFFLINE;
        domain.last = WR_OFFLINE;
        m_hpr->depends.push_back(domain.name);
        Detail d(std::move(domain));
        d.package = package;
        m_hpr->details.push_back(std::move(d));
    }
    m_hpr->details.sort();
    sendControlMessage(CTL_RESET);
    setState(MR_PENDING);
}

void Server::download()
{
    setState(MR_DOWNLOAD);
    m_hpr->webQueue.postEvent(std::make_shared<WebEvent>(WebEvent::REQ_DOWNLOAD));
}

void Server::deploy()
{
    sendControlMessage(CTL_DEPLOY);
    setState(MR_DEPLOY);
}

void Server::cancel()
{
    sendControlMessage(CTL_CANCEL);
    setState(MR_CANCEL);
    if (m_hpr->lastState == MR_IDLE || m_hpr->lastState == MR_PENDING || m_hpr->lastState == MR_READY
        || m_hpr->lastState == MR_READY || m_hpr->lastState == MR_DOWNLOAD || m_hpr->lastState == MR_VERIFY) {
        feedback(true);
    }
}

void Server::feedback(bool finished, int error)
{
    if (m_hpr->hasFeed) {
        LOG_WARNING("has feedback");
        return;
    }
    if (m_hpr->webFeed.type == WebFeed::TP_UNKNOWN) {
        LOG_WARNING("feedback type is unknown");
        return;
    } else if (m_hpr->webFeed.type == WebFeed::TP_DEPLOY) {
        m_hpr->webFeed.id = m_hpr->upgrade.id;
    } else if (m_hpr->webFeed.type == WebFeed::TP_CANCEL) {
        m_hpr->webFeed.id = m_hpr->cancelId;
    }
    m_hpr->webFeed.execution = WebFeed::EXE_CLOSED;
    m_hpr->hasFeed = true;
    m_hpr->errorCode = error;
    ServerState state;
    if (finished) {
        state = MR_DONE_ASK;
        m_hpr->webFeed.result = WebFeed::RS_SUCCESS;
        if (m_hpr->webFeed.type == WebFeed::TP_DEPLOY) {
            LOG_DEBUG("deploy finished");
        } else {
            LOG_DEBUG("cancel finished");
        }
    } else {
        state = MR_ERROR_ASK;
        m_hpr->webFeed.result = WebFeed::RS_FAILURE;
        if (m_hpr->webFeed.type == WebFeed::TP_DEPLOY) {
            LOG_WARNING("deploy error", "(code:", m_hpr->errorCode, ")");
        } else {
            LOG_WARNING("cancel error", "(code:", m_hpr->errorCode, ")");
        }
    }
    sendControlMessage(CTL_CLEAR);
    WebFeed tmpFeed = m_hpr->webFeed;
    tmpFeed.details = getWebFeedDetails();
    tmpFeed.progress = getWebFeedProgress();
    m_hpr->webQueue.postEvent(std::make_shared<WebEvent>(WebEvent::REQ_STOP));
    m_hpr->webQueue.postEvent(std::make_shared<WebFeedEvent>(tmpFeed));
    if (m_hpr->detailSubscribed) {
        setState(state);
    } else {
        idle();
    }
    Utils::freeUnusedMemory();
}

void Server::sendControlMessage(Control control, bool cache)
{
    if (control == CTL_UNKNOWN) {
        return;
    }
    if (m_hpr->control != control) {
        m_hpr->lastControl = m_hpr->control;
        m_hpr->control = control;
    }
    if (!cache) {
        m_hpr->controlMessageId++;
        if (m_hpr->controlMessageId > MIFSA_OTA_MESSAGE_TOTAL_COUNT) {
            m_hpr->controlMessageId = 0;
        }
    }
    ControlMessage controlMessage(m_hpr->controlMessageId, control, upgrade(), depends());
    interface()->sendControlMessage(controlMessage);
}

void Server::sendDetailMessage(bool cache)
{
    if (!cache) {
        m_hpr->detailMessageId++;
        if (m_hpr->detailMessageId > MIFSA_OTA_MESSAGE_TOTAL_COUNT) {
            m_hpr->detailMessageId = 0;
        }
    }
    DetailMessage detailMessage(m_hpr->detailMessageId, state(), lastState(), isActive(), errorCode(), step(), progress(), message(), details());
    interface()->sendDetailMessage(detailMessage);
}

static int getMaxDeployTime(const Detail& d)
{
    int32_t maxDeployTime = d.package.meta.value("max_deploy_time").toInt();
    if (maxDeployTime <= 0) {
        maxDeployTime = d.domain.meta.value("max_deploy_time").toInt();
        if (maxDeployTime <= 0) {
            maxDeployTime = MIFSA_OTA_MAX_DEPLOY_TIME_CLIENT;
        }
    }
    return maxDeployTime;
}

static int getMaxRestartTime(const Detail& d)
{
    int32_t maxRestartTime = d.package.meta.value("max_restart_time").toInt();
    if (maxRestartTime <= 0) {
        maxRestartTime = d.domain.meta.value("max_restart_time").toInt();
        if (maxRestartTime <= 0) {
            maxRestartTime = MIFSA_OTA_MAX_DEPLOY_RESTART_TIME_CLIENT;
        }
    }
    return maxRestartTime;
}

void Server::processDomains()
{
    if (m_hpr->webFeed.type == WebFeed::TP_UNKNOWN) {
        return;
    }
    if (m_hpr->state == MR_IDLE || m_hpr->state == MR_UNKNOWN || m_hpr->state == MR_OFFLINE) {
        return;
    }
    if (m_hpr->hasFeed) {
        return;
    }
    if (m_hpr->errorCode != 0) {
        return;
    }
    if (m_hpr->state == MR_PENDING) {
        uint32_t maxReadyTime = MIFSA_OTA_MAX_PENDING_TIME;
        if (m_hpr->firstStart) {
            maxReadyTime = MIFSA_OTA_MAX_PENDING_TIME_FIRST;
        }
        if (m_hpr->stateElapsed.get() > maxReadyTime) {
            LOG_WARNING("pending time out");
            feedback(false, 1910);
            return;
        }
    } else if (m_hpr->state == MR_DOWNLOAD_ASK || m_hpr->state == MR_DEPLOY_ASK || m_hpr->state == MR_CANCEL_ASK || m_hpr->state == MR_RESUME_ASK) {
        //        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_ASK_TIME) {
        //            LOG_WARNING("ask time out");
        //            feedback(false, 1911);
        //            return;
        //        }
    } else if (m_hpr->state == MR_DONE_ASK || m_hpr->state == MR_ERROR_ASK) {
        //        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_ASK_TIME) {
        //            LOG_WARNING("ask time out");
        //            feedback(false, 1912);
        //            return;
        //        }
    } else if (m_hpr->state == MR_DOWNLOAD || m_hpr->state == MR_DISTRIBUTE) {
        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_TRANSFER_TIME) {
            LOG_WARNING("transfer time out");
            feedback(false, 1913);
            return;
        }
    } else if (m_hpr->state == MR_VERIFY) {
        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_VERIFY_TIME) {
            LOG_WARNING("verify time out");
            feedback(false, 1914);
            return;
        }
    } else if (m_hpr->state == MR_DEPLOY) {
        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_DEPLOY_TIME) {
            LOG_WARNING("deploy time out");
            feedback(false, 1915);
            return;
        }
    } else if (m_hpr->state == MR_CANCEL) {
        if (m_hpr->stateElapsed.get() > MIFSA_OTA_MAX_CANCEL_TIME) {
            LOG_WARNING("cancel time out");
            feedback(false, 1916);
            return;
        }
    }
    int vaildCount = 0;
    int waitCount = 0;
    int equalCount = 0;
    int cancelCount = 0;
    int errorCount = 0;
    bool detailChanged = false;
    for (auto& d : m_hpr->details) {
        if (d.package.domain.empty()) {
            LOG_WARNING("domain package is empty");
            feedback(false, 1917);
            return;
        }
        if (d.detectVersionEqual()) {
            equalCount++;
        }
        if (d.domain.error != 0 || d.domain.state == WR_ERROR) {
            errorCount++;
        }
        if (d.domain.state != WR_OFFLINE && d.heartbeat.active()) {
            if (d.heartbeat.get() > MIFSA_OTA_HEARTBEAT_TIME_OUT) {
                d.domain.last = d.domain.state;
                d.domain.state = WR_OFFLINE;
                detailChanged = true;
            }
        }
        //
        if (m_hpr->state == MR_PENDING || m_hpr->state == MR_READY || m_hpr->state == MR_DOWNLOAD_ASK) {
            if (d.domain.state != WR_UNKNOWN && d.domain.state != WR_OFFLINE && !d.domain.version.empty()) {
                if (d.detectVersionVaild()) {
                    vaildCount++;
                } else {
                    LOG_WARNING("detect version not vaild, when state is pending or ready", "(", d.domain.name, "");
                    feedback(false, 1918);
                    return;
                }
            }
        } else if (m_hpr->state == MR_DEPLOY || m_hpr->state == MR_CANCEL) {
            if (m_hpr->state == MR_CANCEL) {
                if (d.detectVersionEqual()) {
                    LOG_WARNING("find domain finished already, when state is cancel");
                    feedback(false, 1919);
                    return;
                } else {
                    if (d.domain.state == WR_IDLE) {
                        if (d.domain.last == WR_CANCEL) {
                            cancelCount++;
                        }
                    }
                }
            }
            if (m_hpr->state == MR_DEPLOY || m_hpr->lastState == MR_DEPLOY) {
                if (d.deploy.active()) {
                    if ((int32_t)d.deploy.get() > getMaxDeployTime(d)) {
                        LOG_WARNING("deploy time out", "(", d.domain.name, "");
                        feedback(false, 1920);
                        return;
                    }
                    if (d.domain.state == WR_OFFLINE) {
                        if (d.heartbeat.active()) {
                            if ((int32_t)d.heartbeat.get() > getMaxRestartTime(d)) {
                                LOG_WARNING("restart time out", "(", d.domain.name, "");
                                feedback(false, 1921);
                                return;
                            }
                        }
                    }
                    if (d.domain.state == WR_IDLE && d.detectVersionEqual()) {
                        d.deploy.stop();
                        detailChanged = true;
                        m_hpr->depends.clear();
                        m_hpr->depends.shrink_to_fit();
                        for (const auto& dd : m_hpr->details) {
                            if (dd.deploy.active() || dd.domain.state == WR_WAIT) {
                                m_hpr->depends.push_back(dd.domain.name);
                            }
                        }
                        if (m_hpr->control == CTL_DEPLOY) {
                            sendControlMessage(CTL_DEPLOY);
                            WebFeed webFeed(m_hpr->upgrade.id, WebFeed::TP_DEPLOY, WebFeed::EXE_SCHEDULED, WebFeed::RS_SUCCESS, getWebFeedDetails(), getWebFeedProgress());
                            m_hpr->webQueue.postEvent(std::make_shared<WebFeedEvent>(webFeed));
                        }
                    }
                } else if (!d.hasDepends(m_hpr->depends)) {
                    if (d.domain.state == WR_DEPLOY) {
                        d.deploy.start();
                        detailChanged = true;
                    }
                }
            }
        } else if (m_hpr->state == MR_WAIT || m_hpr->state == MR_DEPLOY_ASK) {
            if (d.domain.state == WR_WAIT) {
                waitCount++;
            }
        }
    }
    if (vaildCount != 0 && vaildCount == (int)m_hpr->details.size()) {
        if (m_hpr->state == MR_PENDING) {
            //
            if (m_hpr->hasNewId) {
                if (m_hpr->cacheState == MR_DEPLOY) {
                    setState(MR_RESUME_ASK);
                } else {
                    setState(MR_READY);
                }
            } else {
                if (m_hpr->cancelResume && m_hpr->hasDeployBreaked) {
                    setState(MR_RESUME_ASK);
                } else {
                    setState(MR_READY);
                }
            }
        } else if (m_hpr->state == MR_READY || m_hpr->state == MR_DOWNLOAD_ASK) {
            if (m_hpr->upgrade.download == Upgrade::MTHD_FORCED) {
                download();
            } else if (m_hpr->upgrade.download == Upgrade::MTHD_ATTEMPT && m_hpr->state == MR_READY) {
                setState(MR_DOWNLOAD_ASK);
            }
        }
    } else {
        if (m_hpr->state == MR_DOWNLOAD_ASK) {
            setState(MR_PENDING);
        }
        if (m_hpr->state == MR_PENDING && m_hpr->details.size() > 0) {
            float step = vaildCount * 100.0f / m_hpr->details.size();
            if (m_hpr->step != step) {
                m_hpr->step = step;
                detailChanged = true;
            }
        }
    }
    if (equalCount != 0 && equalCount == (int)m_hpr->details.size()) {
        if (m_hpr->firstStart || m_hpr->state == MR_DEPLOY) {
            feedback(true);
            return;
        }
    }
    if (waitCount != 0 && waitCount == (int)m_hpr->details.size()) {
        if (m_hpr->state == MR_WAIT || m_hpr->state == MR_DEPLOY_ASK) {
            if (m_hpr->upgrade.deploy == Upgrade::MTHD_FORCED) {
                deploy();
            } else if (m_hpr->upgrade.deploy == Upgrade::MTHD_ATTEMPT && m_hpr->state == MR_WAIT) {
                setState(MR_DEPLOY_ASK);
            }
        }
    } else {
        if (m_hpr->state == MR_DEPLOY_ASK) {
            setState(MR_WAIT);
        }
    }
    if (cancelCount != 0 && cancelCount == (int)m_hpr->details.size()) {
        if (m_hpr->state == MR_CANCEL) {
            feedback(true);
            return;
        }
    }
    if (errorCount > 0) {
        LOG_WARNING("find domain error in final");
        feedback(false, 1950);
        return;
    }
    if (detailChanged) {
        sendDetailMessage();
    }
}
}

MIFSA_NAMESPACE_END
