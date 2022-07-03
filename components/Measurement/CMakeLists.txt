# Building the Introspection-Measurement component
# July 1 2022
# Michael Neises

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_gcc_cross)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_usr)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include_usr/x86_64-linux-gnu)

# compiling hacl
set(hacl_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../hacl-star/dist")
set(hacl_path "${hacl_dir}/gcc-compatible")
set(kremlin_path "${hacl_dir}/kremlin/include")
set(kremlib_path "${hacl_dir}/kremlin/kremlib/dist/minimal")

include_directories(${hacl_path})
include_directories(${kremlin_path})
include_directories(${kremlib_path})

set(hacl_sources ${hacl_path}/Hacl_Hash.c)
add_library(hacl STATIC ${hacl_sources})
target_link_libraries(hacl)

DeclareCAmkESComponent(
    introspect_memdev
    SOURCES
        introspect_component.c
    INCLUDES
        include_gcc_cross
        ${hacl_path} ${kremlin_path} ${kremlib_path}
    LIBS
        hacl
)
