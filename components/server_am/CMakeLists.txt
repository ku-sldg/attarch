
# see CakeMLHelper.cmake in am-cakeml
# surely these can be consolidated.
# for some reason, am-cakeml does not set the target flag even though
# the target_arch flag has been set
set(IS_CAMKES_AM true)
set(CAKE_FLAGS "--target=arm8")
set(TARGET_ARCH "armv8")

# TODO:
# right now, only the CAmkES_server app is set to build.
# Can we change that back...?
# Or perhaps can we allow an option to be passed here, like:
# set(app camkes_server)
# or something?

set(am_cakeml_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../am-cakeml")
add_subdirectory(../../am-cakeml "${CMAKE_CURRENT_BINARY_DIR}/am-cakeml")

#get_files(man_comp_src ${server_am_src} ../tests/DemoFiles/Kim/Test_Am_Lib_Kim.sml ../apps/ManifestCompiler/Client.sml )

