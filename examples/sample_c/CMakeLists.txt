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

project(mifsa_${MIFSA_MODULE_TYPE}_example_sample_c LANGUAGES C)

add_executable(
     ${PROJECT_NAME}
    "sample.c"
    )
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mifsa_base
    mifsa_${MIFSA_MODULE_TYPE}
    mifsa_${MIFSA_MODULE_TYPE}_c
    )
install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    )
