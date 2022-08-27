# MIFSA-OTA

The OTA module in [mifsa](https://github.com/lujuntuan/mifsa).

## So what is this about? (to be modified)
整车OTA升级的解决方案，是基于SOA架构的整车域控制器的升级，包含OTA-Master跟OTA-Worker两部分：
- OTA云服务器采用的是Eclipsed的hawkbit，可以基于Docker灵活部署
- OTA-Master 是升级主控，其主要负责检测Web端的升级动作、版本对比、升级包下载、校验、差分生成、各个OTA-Worker的控制和状态管理、升级包分发、状态上报web、容灾机制等功能
- OTA-Worker 是Domain升级分控，其主要负责OTA-Master状态指令获取，版本上报、升级包下载、校验、状态上报等，另外其也可以订阅整个主控的升级状态和各个分控的状态

## Features  (to be modified)
- 基于SOA的架构设计
- 基于Modern C++的base库封装，主要有线程队列、信号量、定时器、线程池、Json解析等等
- CMake工程管理
- Yocto-layer适配
- RPC层抽象接口封装
- 基于ROS2的RPC实现
- 基于VSOMEIP的RPC实现
- 基于FDBUS的RPC实现
- https通讯及传输
- 基于线程池的http_server用以升级包分发
- 容灾机制实现
- 基于Docker的Hawkbit云部署
- Openssl http签名验证
- OTA-Master的开发实现
- OTA-Worker的开发实现
- OTA-Viewer的实现，用于订阅整个Domain的升级状态和获取Master的状态
- Hawkbit云部署及Openssl签名验证
- Oss-Fuzz模糊测试

## Requirements:

- [mifsa-base](https://github.com/lujuntuan/mifsa-base)

One of the following rpc communication libraries is required: 

- [fdbus](https://gitee.com/jeremyczhen/fdbus) [protobuf](https://github.com/protocolbuffers/protobuf)

- [commonapi-core-runtime](https://github.com/COVESA/capicxx-core-runtime) [commonapi-someip-runtime](https://github.com/COVESA/capicxx-someip-runtime) [capicxx-core-tools](https://github.com/COVESA/capicxx-core-tools) [capicxx-someip-tools](https://github.com/COVESA/capicxx-someip-tools)

- [rclcpp(ros2)](https://github.com/ros2/rclcpp)

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

  Select the soa communication component (idl), support auto/ros/vsomeip/fdbus, default is auto.

Examples:

```shell
cmake -B build \
	-DCMAKE_INSTALL_PREFIX=build/install \
	-DMIFSA_BUILD_EXAMPLES=ON \
	-DMIFSA_BUILD_TESTS=OFF \
	-DMIFSA_IDL_TYPE=fdbus
cmake --build build --target install
```

```shell
cmake -B build \
	-DCMAKE_INSTALL_PREFIX=build/install \
	-DMIFSA_BUILD_EXAMPLES=ON \
	-DMIFSA_BUILD_TESTS=OFF \
	-DMIFSA_IDL_TYPE=fdbus
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
find_package(mifsa_base REQUIRED)
find_package(mifsa_ota REQUIRED)
target_link_libraries(
    [TARGET]
    mifsa_base
    mifsa_ota
    )
...
```

In cpp code:

```c++

```

## Copyright:

Juntuan.Lu, 2020-2030, All rights reserved.
