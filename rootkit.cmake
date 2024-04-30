
# Add the external diamorphine module project
ExternalProject_Add(diamorphine-module
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/diamorphine-module
    BUILD_ALWAYS ON
    STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/diamorphine-module-stamp
    EXCLUDE_FROM_ALL
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${LINUX_64BIT_TOOLCHAIN}
        -DLINUX_KERNEL_DIR=${linux_dir}
        -DMODULE_HELPERS_FILE=${CAMKES_VM_LINUX_DIR}/linux-module-helpers.cmake
)
# Add our module binary to the overlay
AddExternalProjFilesToOverlay(diamorphine-module
${CMAKE_CURRENT_BINARY_DIR}/diamorphine-module overlay "lib/modules/4.9.307/kernel/drivers/vmm"
    FILES diamorphine.ko)

