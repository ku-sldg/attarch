cmake_minimum_required(VERSION 3.8.2)

if(NOT MODULE_HELPERS_FILE)
    message(FATAL_ERROR "MODULE_HELPERS_FILE is not defined")
endif()

include("${MODULE_HELPERS_FILE}")

DefineLinuxModule(${CMAKE_CURRENT_LIST_DIR}/poison poison-module poison-target KERNEL_DIR ${LINUX_KERNEL_DIR})
DefineLinuxModule(${CMAKE_CURRENT_LIST_DIR}/connection connection-module connection-target KERNEL_DIR ${LINUX_KERNEL_DIR})
#DefineLinuxModule(${CMAKE_CURRENT_LIST_DIR}/diamorphine diamorphine-module diamorphine-target KERNEL_DIR ${LINUX_KERNEL_DIR})
