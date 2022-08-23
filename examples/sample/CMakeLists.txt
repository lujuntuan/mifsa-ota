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

project(mifsa_${MIFSA_MODULE_TYPE}_example_sample LANGUAGES CXX)

add_executable(
     ${PROJECT_NAME}
    "sample.cpp"
    )
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mifsa_base
    mifsa_${MIFSA_MODULE_TYPE}
    )
install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    )

if(LINUX)
    add_custom_command(
        TARGET
        ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/*.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    install(
        FILES
        ${PROJECT_SOURCE_DIR}/*.sh
        DESTINATION
        share/${CMAKE_INSTALL_BINDIR}
        )
endif()
