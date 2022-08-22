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

project(examples)

if (TARGET mifsa_${MIFSA_MODULE_TYPE})
    add_subdirectory(sample)
    option(MIFSA_OTA_EXAMPLE_USE_QT6 "Use Qt6 library" OFF)
    if(MIFSA_OTA_EXAMPLE_USE_QT6)
        find_package(QT NAMES Qt6 QUIET)
    else()
        find_package(QT NAMES Qt5 QUIET)
    endif()
    if(QT_VERSION_MAJOR)
        find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS
            Core Widgets
        )
    endif()
    if(Qt${QT_VERSION_MAJOR}_FOUND)
        add_subdirectory(viewer)
    endif()
endif()
if (TARGET mifsa_${MIFSA_MODULE_TYPE}_c)
    add_subdirectory(sample_c)
endif()
