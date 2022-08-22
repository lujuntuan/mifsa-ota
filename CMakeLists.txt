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
    if(NOT MIFSA_BASE_CMAKE)
        include(${CMAKE_CURRENT_LIST_DIR}/../mifsa-base/cmake/base.cmake)
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

add_subdirectory(idls)
add_subdirectory(server)
add_subdirectory(client)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/platforms)
    #add_subdirectory(platforms)
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/c)
    #add_subdirectory(c)
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/examples AND MIFSA_BUILD_EXAMPLES)
    #add_subdirectory(examples)
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests AND MIFSA_BUILD_TESTS)
    add_subdirectory(tests)
endif()
