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

project(mifsa_${MIFSA_MODULE_TYPE}_example_sample LANGUAGES CXX)

add_executable(
     ${PROJECT_NAME}
    "sample.cpp"
    )
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mifsa_${MIFSA_MODULE_TYPE}
    )
install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME
    DESTINATION
    ${CMAKE_INSTALL_BINDIR}
    )

if(LINUX)
    file(GLOB_RECURSE
        SAMPLE_SCRIPTS
        ${PROJECT_SOURCE_DIR}/*.sh
        )
    add_custom_target(
        ${PROJECT_NAME}_scripts
        SOURCES
        ${SAMPLE_SCRIPTS}
        )
    add_custom_command(
        TARGET
        ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SAMPLE_SCRIPTS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    install(
        FILES
        ${SAMPLE_SCRIPTS}
        PERMISSIONS
        OWNER_EXECUTE
        DESTINATION
        ${CMAKE_INSTALL_BINDIR}
        )
endif()
