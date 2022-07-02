# Building the Introspection-Measurement component
# July 1 2022
# Michael Neises

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/include_gcc_cross)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/include_usr)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/include_usr/x86_64-linux-gnu)

# compiling hacl
set(hacl_dir "${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/hacl-star/dist")
set(hacl_path "${hacl_dir}/gcc-compatible")
set(karamel_path "{hacl_dir}/karamel/include")
set(karalib_path "${hacl_dir}/karamel/krmllib/dist/minimal")

# not sure if these three includes are necessary
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/hacl-star/dist/gcc-compatible)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/hacl-star/dist/karamel/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/components/Measurement/hacl-star/dist/karamel/krmllib/dist/minimal)

set(hacl_sources ${hacl_path}/Hacl_Hash_SHA2.c)
add_library(hacl STATIC ${hacl_sources})
target_link_libraries(hacl)

DeclareCAmkESComponent(
    introspect_memdev
    SOURCES
        components/Measurement/introspect_component.c
    INCLUDES
        components/Measurement/includes
        components/Measurement/hacl-star/dist/gcc-compatible
        components/Measurement/hacl-star/dist/karamel/include
        components/Measurement/hacl-star/dist/karamel/krmllib/dist/minimal
    LIBS
        hacl
)
