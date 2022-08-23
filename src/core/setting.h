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

#include "mifsa/ota/config.h"

#ifndef MIFSA_OTA_CORE_SETTING_H
#define MIFSA_OTA_CORE_SETTING_H

#define MIFSA_OTA_ENABLE_MESSAGE_HTTP
#define MIFSA_OTA_ENABLE_PULL_HTTP
#define MIFSA_OTA_ENABLE_DOWNLOAD_HTTP
#define MIFSA_OTA_ENABLE_DISTRIBUTE_HTTP
#define MIFSA_OTA_ENABLE_VERIFY_OPENSSL
#define MIFSA_OTA_ENABLE_PATCH_BSDIFF

#define MIFSA_OTA_QUEUE_ID_SERVER 1
#define MIFSA_OTA_QUEUE_ID_CLIENT 2
#define MIFSA_OTA_QUEUE_ID_WEB 3

#define MIFSA_OTA_WEB_USE_POLLING 0
#define MIFSA_OTA_WEB_CHECK_INTERVAL 10000
#define MIFSA_OTA_WEB_TRANSFER_INTERVAL 500
#define MIFSA_OTA_WEB_TRANSFER_INTERVAL_MIN 50
#define MIFSA_OTA_WEB_TIMEOUT 60000
#define MIFSA_OTA_WEB_TRANSFER_TEST_TIME 0
#define MIFSA_OTA_WEB_PULL_FUNC_NAME "mifsa_ota"

#define MIFSA_OTA_MAX_PENDING_TIME 60000 * 5
#define MIFSA_OTA_MAX_PENDING_TIME_FIRST 60000 * 60
#define MIFSA_OTA_MAX_ASK_TIME 60000 * 5
#define MIFSA_OTA_MAX_CANCEL_TIME 60000 * 60
#define MIFSA_OTA_MAX_VERIFY_TIME 60000 * 5
#define MIFSA_OTA_MAX_TRANSFER_TIME 60000 * 60 * 6
#define MIFSA_OTA_MAX_DEPLOY_TIME 60000 * 60 * 6
#define MIFSA_OTA_MAX_DEPLOY_TIME_CLIENT 60000 * 60
#define MIFSA_OTA_MAX_DEPLOY_RESTART_TIME_CLIENT 60000 * 1

#define MIFSA_OTA_PROCESS_DOMAIN_TIME 1000
#define MIFSA_OTA_HEARTBEAT_TIME_OUT 6000
#define MIFSA_OTA_MESSAGE_TOTAL_COUNT 100000
#define MIFSA_OTA_DOWNLOAD_KEEP_FILE_COUNT 2
#define MIFSA_OTA_RETRY_TIMES 3

#endif // MIFSA_OTA_CORE_SETTING_H
