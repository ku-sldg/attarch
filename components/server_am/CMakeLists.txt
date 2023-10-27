
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
set(build_dir "${am_cakeml_dir}/build")

# Ensure the build directory exists
file(MAKE_DIRECTORY ${build_dir})

# Configure the project
execute_process(
    COMMAND ${CMAKE_COMMAND} ..
    WORKING_DIRECTORY ${build_dir}
    RESULT_VARIABLE result
)
if(NOT result EQUAL "0")
    message(FATAL_ERROR "Configuration of am-cakeml failed")
endif()

# Build manifest_generator and manifest_compiler
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target manifest_generator manifest_compiler
    WORKING_DIRECTORY ${build_dir}
    RESULT_VARIABLE result
)
if(NOT result EQUAL "0")
    message(FATAL_ERROR "Build of am-cakeml failed")
endif()

