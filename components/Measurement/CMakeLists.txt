# Building the Introspection-Measurement component
# 28 April 2023
# Michael Neises

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
include_directories(/usr/lib/gcc-cross/aarch64-linux-gnu/10/include)

include(../../am-cakeml/system/crypto/evercrypt/hacl.cmake)

DeclareCAmkESComponent(
    introspect_memdev
    SOURCES
        introspect_component.c
        hash.c
        KnownDigests.c
    INCLUDES
        /usr/lib/gcc-cross/aarch64-linux-gnu/10/include
)

add_hacl_camkes(introspect_memdev)

