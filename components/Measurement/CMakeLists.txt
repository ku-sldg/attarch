# Building the Introspection-Measurement component
# July 1 2022
# Michael Neises

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_gcc_cross)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_usr)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_usr/x86_64-linux-gnu)

DeclareCAmkESComponent(
    introspect_memdev
    SOURCES
        ${hacl_sources}
        introspect_component.c
        #hash.c
        KnownDigests.c
    INCLUDES
        include_gcc_cross
        ${hacl_path} ${kremlin_path} ${kremlib_path}
    # LIBS
    #     hacl
)



