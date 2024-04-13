#
# Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(camkes-vm-introspect C)

include(${CAMKES_ARM_VM_HELPERS_PATH})
find_package(camkes-vm-linux REQUIRED)
include(${CAMKES_VM_LINUX_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_SOURCE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(ExternalProject)
include(external-project-helpers)


# ==================================================================================================
# SECTION do this stuff because always qemu-arm-virt
# ==================================================================================================
set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
include(simulation)
set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")
if(SIMULATION)
    GenerateSimulateScript()
endif()
# ==================================================================================================
# END SECTION
# ==================================================================================================

AddToFileServer("linux" "${CMAKE_CURRENT_SOURCE_DIR}/linux-stable/arch/arm64/boot/Image")

#set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs_crossvm.cpio.gz")
set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/qemu-arm-virt/rootfs.cpio.gz")

# ==================================================================================================
# SECTION compile linux kernel modules
# ==================================================================================================

# Setup and Configure Linux Sources
set(linux_config "${CMAKE_CURRENT_SOURCE_DIR}/linux-stable/.config")
set(linux_symvers "${CMAKE_CURRENT_SOURCE_DIR}/linux-stable/Module.symvers")

# linux_dir should point to wherever you built the kernel
set(linux_dir "${CMAKE_CURRENT_SOURCE_DIR}/linux-stable")

# Add the external connection module project
ExternalProject_Add(connection-module
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/connection-module
    BUILD_ALWAYS ON
    STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/connection-module-stamp
    EXCLUDE_FROM_ALL
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${LINUX_64BIT_TOOLCHAIN}
        -DLINUX_KERNEL_DIR=${linux_dir}
        -DMODULE_HELPERS_FILE=${CAMKES_VM_LINUX_DIR}/linux-module-helpers.cmake
)
# Add our module binary to the overlay
AddExternalProjFilesToOverlay(connection-module
${CMAKE_CURRENT_BINARY_DIR}/connection-module overlay "lib/modules/4.9.307/kernel/drivers/vmm"
    FILES connection.ko)

# Add the external poison module project
ExternalProject_Add(poison-module
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/poison-module
    BUILD_ALWAYS ON
    STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/poison-module-stamp
    EXCLUDE_FROM_ALL
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${LINUX_64BIT_TOOLCHAIN}
        -DLINUX_KERNEL_DIR=${linux_dir}
        -DMODULE_HELPERS_FILE=${CAMKES_VM_LINUX_DIR}/linux-module-helpers.cmake
)
# Add our module binary to the overlay
AddExternalProjFilesToOverlay(poison-module
${CMAKE_CURRENT_BINARY_DIR}/poison-module overlay "lib/modules/4.9.307/kernel/drivers/vmm"
    FILES poison.ko)

# ==================================================================================================
# END SECTION
# ==================================================================================================

# ==================================================================================================
# SECTION build linux user apps
# ==================================================================================================

ExternalProject_Add(
    dataport-app
    URL
    file:///${CMAKE_CURRENT_SOURCE_DIR}/dataport_src
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/dataport_src
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

# Assuming AddExternalProjFilesToOverlay is a macro or function you've defined elsewhere
AddExternalProjFilesToOverlay(dataport-app
    ${CMAKE_CURRENT_BINARY_DIR} overlay "usr/bin"
    FILES dataport_src/dataportAPI)

# Cross-compile am-cakeml app "vm_client" for the VM
ExternalProject_Add(
    useram
    URL
    file:///${CMAKE_CURRENT_SOURCE_DIR}/am-cakeml
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/am-cakeml
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DIS_VM_USERAM=true
    -C ${CMAKE_CURRENT_SOURCE_DIR}/am-cakeml/camkes_apps/client/config.cmake
)
AddExternalProjFilesToOverlay(
    useram
    ${CMAKE_CURRENT_BINARY_DIR}/am-cakeml/camkes_apps/client/
    overlay
    "/useram"
    FILES
    useram
)

# This project is the simplest test that the introspection capabilities
# are still live.
ExternalProject_Add(
    introspect-app
    URL
    file:///${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-apps/camkes-introspect-app/pkgs/introspect
    BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/introspect-app
    INSTALL_COMMAND
    ""
    BUILD_ALWAYS
    ON
    EXCLUDE_FROM_ALL
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

AddExternalProjFilesToOverlay(
    introspect-app
    ${CMAKE_CURRENT_BINARY_DIR}/introspect-app
    overlay
    "usr/bin"
    FILES
    introspect
)
# ==================================================================================================
# END SECTION
# ==================================================================================================


# Add script to initialise dataport module
AddFileToOverlayDir(
    "S90crossvm_module_init"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/introspect_dataport_init
    "etc/init.d"
    overlay
)

AddFileToOverlayDir(
    "S91crossvm_test"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/introspect_vm_test
    "etc/init.d"
    overlay
)

# Add requisite files for the useram: manifest, private key, term file.
AddFileToOverlayDir(
    "manifest.json"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/user_am_files/FormalManifest_P0.json
    "useram"
    overlay
)
AddFileToOverlayDir(
    "private.key"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/user_am_files/Test_PrivKey
    "useram"
    overlay
)
AddFileToOverlayDir(
    "term.json"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/user_am_files/ClientCvmTermCm.json
    "useram"
    overlay
)

# Construct new rootfs
AddOverlayDirToRootfs(
    overlay
    ${rootfs_file}
    "buildroot"
    "rootfs_install"
    output_overlayed_rootfs_location
    rootfs_target
    GZIP
)

AddToFileServer("linux-initrd" ${output_overlayed_rootfs_location} DEPENDS rootfs_target)

AddCamkesCPPFlag(
    cpp_flags
    CONFIG_VARS
    VmEmmc2NoDMA
    VmVUSB
    VmVchan
    Tk1DeviceFwd
    Tk1Insecure
    VmVirtioNetVirtqueue
)

DefineCAmkESVMFileServer()

CAmkESAddImportPath(${KernelARMPlatform})

# Define our VM Component with out cross vm dataports glue code
DeclareCAmkESComponent(VM SOURCES src/cross_vm_connection.c src/init_dataport_ram.c)

add_subdirectory(components/Measurement)
add_subdirectory(components/server_am)
add_subdirectory(components/FileServer)
add_subdirectory(components/LinuxHandler)

# Declare root server
DeclareCAmkESRootserver(
    vm_introspect.camkes
    CPP_FLAGS
    ${cpp_flags}
    CPP_INCLUDES
    ${CAMKES_VM_DIR}/components/VM_Arm
)
