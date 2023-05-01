# Building the Introspection-Measurement component
# 28 April 2023
# Michael Neises

include(../../am-cakeml/system/crypto/evercrypt/hacl.cmake)

DeclareCAmkESComponent(
    introspect_memdev
    SOURCES
        introspect_component.c
        hash.c
        KnownDigests.c
)

add_hacl_camkes(introspect_memdev)

