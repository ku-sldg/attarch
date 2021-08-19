set(CAmkES true)

get_files(platam_src ${cop_src} ../../am/CommTypes.sml
    ../../system/camkes/dataports/DataportFFI.sml PlatAM.sml)
get_files(platam_c_src ../../system/camkes/dataports/dataport_ffi.c)

# Needs the case-versioned cake compiler
build_cake("platam.cake" SOURCES "${platam_src}" ENTRY_NAME run)
DeclareCAmkESComponent(
    Platam 
    SOURCES ${platam_c_src}
    LIBS platam.cake camkescakeml
)