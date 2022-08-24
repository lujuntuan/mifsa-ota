# MIFSA-OTA

The OTA module in [mifsa](https://github.com/lujuntuan/mifsa).

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
