#*********************************************************************************
#  *Copyright(C): Juntuan.Lu 2021
#  *Author:  Juntuan.Lu
#  *Version: 1.0
#  *Date:  2022/04/01
#  *Phone: 15397182986
#  *Description:
#  *Others:
#  *Function List:
#  *History:
#**********************************************************************************

project(mifsa_${MIFSA_MODULE_TYPE}_example_viewer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(
     ${PROJECT_NAME}
    "viewer.cpp"
    "mainwindow.cpp"
    "mainwindow.h"
    "mainwindow.ui"
    )
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mifsa_base
    mifsa_${MIFSA_MODULE_TYPE}
    Qt${QT_VERSION_MAJOR}::Widgets
    )
install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    )
