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

#include <mifsa/base/thread.h>
#include <mifsa/ota/client.h>
#include <random>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    Ota::Client client(argc, argv);
    // dcus_client_engine->setMeta({ { "max_deploy_time", 8000 } });
    client.setAttribute({ { "hw_addr", "abcdefg" } });
    client.setVersion("1.0.0");
    client.subscibeDeploy(
        [&](const std::string& dir, const Ota::Client::FilePaths filePaths) {
            for (int i = 0; i < 100; i++) {
                if (client.hasStopAction()) {
                    return;
                }
                if (client.hasCancelAction()) {
                    client.postCancelDone(true);
                    return;
                }
                client.postDeployProgress(
                    (float)i, std::string("Deploy progress = ") + std::to_string(i));
                static std::random_device sd;
                static std::minstd_rand linearRan(sd());
                static std::uniform_int_distribution<unsigned> round(1, 600);
                Thread::sleepMilli(round(linearRan));
            }
            client.postDeployDone(true);
        });
    // deploy about 30s
    int reval = client.exec();
    return reval;
}
