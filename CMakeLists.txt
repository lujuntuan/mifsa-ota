#*********************************************************************************
#  *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
#  *Author:  Juntuan.Lu
#  *Version: 1.0
#  *Date:  2022/04/01
#  *Email: 931852884@qq.com
#  *Description:
#  *Others:
#  *Function List:
#  *History:
#**********************************************************************************

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MIFSA_MODULE_TYPE ota)
file(STRINGS version.txt MIFSA_MODULE_VERSION)
project(mifsa_${MIFSA_MODULE_TYPE} VERSION ${MIFSA_MODULE_VERSION} LANGUAGES C CXX)
#set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build to Release and Debug")
#set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Where install")

set(MIFSA_INTERNAL_PROJECT ON)
set(MIFSA_MODULE_PROJECT ON)
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/../mifsa-base/cmake/base.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../mifsa-base/cmake/base.cmake)
    if(NOT MIFSA_BASE_CMAKE)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../mifsa-base ${CMAKE_BINARY_DIR}/mifsa-base)
    endif()
else()
    find_package(mifsa_base REQUIRED)
endif()

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE
    PROJECT_HEADERS
    ${PROJECT_SOURCE_DIR}/include/*.h
    )
add_custom_target(
    ${PROJECT_NAME}_headers
    SOURCES
    ${PROJECT_HEADERS}
    )

mifsa_install_etc(mifsa_${MIFSA_MODULE_TYPE}_root_etc ${PROJECT_SOURCE_DIR})
mifsa_install_share(mifsa_${MIFSA_MODULE_TYPE}_root_share ${PROJECT_SOURCE_DIR})

option(MIFSA_OTA_BUILD_SERVER "Build ${MIFSA_MODULE_TYPE} server" ON)
option(MIFSA_OTA_BUILD_CLIENT "Build ${MIFSA_MODULE_TYPE} client" ON)
option(MIFSA_OTA_ENABLE_HTTPS "ota module use https" OFF)
if(MIFSA_IDL_SUPPORT)
    add_subdirectory(idls)
endif()
if(MIFSA_OTA_BUILD_SERVER)
    message("** Build ${MIFSA_MODULE_TYPE} server")
    add_subdirectory(server)
endif()
if(MIFSA_OTA_BUILD_CLIENT)
    message("** Build ${MIFSA_MODULE_TYPE} client")
    add_subdirectory(client)
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/c)
        add_subdirectory(c)
    endif()
endif()
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/platforms)
    add_subdirectory(platforms)
endif()
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/examples AND MIFSA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests AND MIFSA_BUILD_TESTS)
    add_subdirectory(tests)
endif()
