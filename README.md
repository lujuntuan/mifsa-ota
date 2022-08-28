# MIFSA-OTA

The OTA module in [mifsa](https://github.com/lujuntuan/mifsa).

Detailed documentation is found in [introduction](doc/introduction-zh.md)(中文).

## Features

- Support batch upgrade, delayed upgrade, trigger upgrade, cancel upgrade and other operations
- Support to build hawkbit cloud service through docker
- Supports resuming from a breakpoint
- Support multi-threaded download
- Support thread pool server distribution
- Support soa architecture
- Support domain controller status reporting
- Support subscription to vehicle upgrade status
- Supports recovery of upgrade exceptions
- Support c/c++ interface
- Support idl service interface providing rpc
- Support OTA-master self-upgrade
- Support differential upgrade

## Requirements:

- [mifsa-base](https://github.com/lujuntuan/mifsa-base)
- [openssl](https://github.com/openssl/openssl)

One of the following rpc communication libraries is required: 

- [fdbus](https://gitee.com/jeremyczhen/fdbus) [protobuf](https://github.com/protocolbuffers/protobuf)

- [commonapi-core-runtime](https://github.com/COVESA/capicxx-core-runtime) [commonapi-someip-runtime](https://github.com/COVESA/capicxx-someip-runtime) [capicxx-core-tools](https://github.com/COVESA/capicxx-core-tools) [capicxx-someip-tools](https://github.com/COVESA/capicxx-someip-tools)

- [rclcpp(ros2)](https://github.com/ros2/rclcpp)

Note: Also supports custom rpc.

mifsa_ota_examples_viewer needs:

- [QtWidgets](https://github.com/qt/qtbase)

## How to build:

```cmake
cmake -B build
cmake --build build --target install
```

Optional:

- -DMIFSA_BUILD_EXAMPLES: 

  whether to compile the examples, default is on.

- -DMIFSA_BUILD_TESTS :

  whether to compile the tests, default is on.

- -DMIFSA_IDL_TYPE: 

  Select the soa communication component (idl), support auto/ros/vsomeip/fdbus/custom, default is auto.
  
- -DMIFSA_OTA_ENABLE_HTTPS:

  Whether to enable https authentication, including web and local, default is off.
  

Examples:

```shell
cmake -B build \
	-DCMAKE_INSTALL_PREFIX=build/install \
	-DMIFSA_BUILD_EXAMPLES=ON \
	-DMIFSA_BUILD_TESTS=OFF \
	-DMIFSA_IDL_TYPE=fdbus \
	-DMIFSA_OTA_ENABLE_HTTPS=OFF
cmake --build build --target install -j8
```

```shell
Note:
For vsomeip support, capicxx-core-tools Need to link to the bin directory.
Example:
ln -sf [capicxx-core-tools dir]/commonapi-core-generator-linux-x86_64 /usr/local/bin/capicxx-core-gen
ln -sf [capicxx-core-tools dir]/commonapi-someip-generator-linux-x86_64 /usr/local/bin/capicxx-someip-gen

For ros2 support, you should set following environment
export AMENT_PREFIX_PATH=[ros2 install dir] #example: opt/ros2_install
export CMAKE_PREFIX_PATH=[ros2 install dir] #example: opt/ros2_install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH;[ros2 lib install dir] #example: opt/ros2_install/lib
export PYTHONPATH=[ros2 python install path] #example: opt/ros2_install/lib/python3.10/site-packages
```

## How to use:

In CMakeLists.txt:

```cmake
...
find_package(mifsa_ota REQUIRED)
target_link_libraries(
    [TARGET]
    mifsa_ota
    )
...
```

In cpp code:

```c++
#include <mifsa/base/thread.h>
#include <mifsa/ota/client.h>
#include <random>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    Ota::Client client(argc, argv);
    // client.setMeta({ { "max_deploy_time", 8000 } });
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
    client.subscibeDetail([&window](const Mifsa::Ota::DetailMessage& detail, bool stateChanged) {
        //TODO
    });
    int reval = client.exec();
    return reval;
}
```

Note: For more usage, please see the code in the [examples](examples) folder.

## How to run

This is a private hawkbit cloud service: [https://dcus.club](https://dcus.club)

Note: username `dcus`, password `dcus1qaz@WSX`

For example:

```shell
master_host> mifsa_ota_server -u https://dcus.club -t a16c62009a414d85043b19be9182fde0
worker_host> mifsa_ota_client -n [domain_id]
```

You can also run the tests:

```shell
mifsa_ota_server -u https://dcus.club -t a16c62009a414d85043b19be9182fde0 -o
mifsa_ota_example_sample_test.sh
```

## Copyright:

Juntuan.Lu, 2020-2030, All rights reserved.
