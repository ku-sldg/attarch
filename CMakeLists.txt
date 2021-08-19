
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(attarch C)

include(${CAMKES_ARM_VM_HELPERS_PATH})
find_package(camkes-vm-linux REQUIRED)
include(${CAMKES_VM_LINUX_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_SOURCE_HELPERS_PATH})
include(${CAMKES_VM_LINUX_MODULE_HELPERS_PATH})
include(ExternalProject)
include(external-project-helpers)

# Create our CPP Flags based on ARM VM config variables
if("${KernelARMPlatform}" STREQUAL "exynos5422")
    set(cpp_flags "-DKERNELARMPLATFORM_EXYNOS5422")
    set(linux_repo "https://github.com/hardkernel/linux.git")
    set(linux_tag "4.14.87-153")
    set(linux_arch "arm")
    set(linux_cross_compile "arm-linux-gnueabi-")
elseif("${KernelARMPlatform}" STREQUAL "qemu-arm-virt")
    set(cpp_flags "-DKERNELARMPLATFORM_QEMU-ARM-VIRT")
    set(linux_repo "https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git")
    set(linux_tag "v4.9.189")
    set(linux_arch "arm64")
    set(linux_cross_compile "aarch64-linux-gnu-")
    include(simulation)
    set(SIMULATION ON CACHE BOOL "Generate simulation script to run qemu with the proper arguments")
    if(SIMULATION)
        GenerateSimulateScript()
    endif()
endif()

AddToFileServer("linux" "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux")

set(rootfs_file "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/rootfs.cpio.gz")
# Checkout and configure linux to build crossvm module
ExternalProject_Add(
    checkout_linux
    GIT_REPOSITORY
    ${linux_repo}
    GIT_TAG
    ${linux_tag}
    GIT_SHALLOW
    1
    GIT_PROGRESS
    1
    BUILD_COMMAND
    ""
    INSTALL_COMMAND
    ""
    CONFIGURE_COMMAND
    ""
    SOURCE_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
)
# Linux config and symvers are to be copied to unpacked archive
set(linux_config "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux_configs/config")
set(linux_symvers "${CAMKES_VM_IMAGES_DIR}/${KernelARMPlatform}/linux_configs/Module.symvers")
# Configure unpacked archive with config and symvers
ConfigureLinux(
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
    ${linux_config}
    ${linux_symvers}
    configure_vm_linux
    ARCH
    ${linux_arch}
    CROSS_COMPILE
    ${linux_cross_compile}
    DEPENDS
    checkout_linux
)

# Compile CrossVM Dataport Module
DefineLinuxModule(
    ${CAMKES_VM_LINUX_DIR}/camkes-linux-artifacts/camkes-linux-modules/camkes-connector-modules/connection
    output_module
    output_module_target
    KERNEL_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/linux_out
    ARCH
    ${linux_arch}
    CROSS_COMPILE
    ${linux_cross_compile}
    DEPENDS
    checkout_linux
    configure_vm_linux
)
AddFileToOverlayDir(
    "connection.ko"
    ${output_module}
    "lib/modules/4.14.87/kernel/drivers/vmm"
    overlay
    DEPENDS
    output_module_target
)

# Add script to initialise dataport module
AddFileToOverlayDir(
    "S90crossvm_module_init"
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay_files/init_scripts/cross_vm_module_init
    "etc/init.d"
    overlay
)

# Cross-compile useram binaries for the VM
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
      -DAPP=userAM
      -DSTATIC_LINKING=on
      -DBASIS=/cake-x64-64/basis_ffi.c
      -DTARGET_ARCH=armv8
      -DCAKE_FLAGS=--target=arm8;--stack_size=10;--heap_size=10
)

AddExternalProjFilesToOverlay(
    useram 
    ${CMAKE_CURRENT_BINARY_DIR}/am-cakeml/apps/userAM/
    overlay
    "/usr/bin"
    FILES
    useram
    client
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
DeclareCAmkESComponent(VM SOURCES src/cross_vm_connections.c)

# Define our demo component that shares a dataport with the VM
add_subdirectory(components/Platam)

# Declare root server
DeclareCAmkESRootserver(
    attarch.camkes
    CPP_FLAGS
    ${cpp_flags}
    CPP_INCLUDES
    ${CAMKES_VM_DIR}/components/VM_Arm
)
