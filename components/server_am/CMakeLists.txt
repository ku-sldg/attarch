#set(APP "platAM")
# TODO: set target based on CMAKE arch value
#set(CAKE_FLAGS "--target=arm8 --stack_size=10 --heap_size=10" CACHE STRING "Arguments passed to the CakeML compiler")


# I really want this to just compile the am-cakeml app called
# CAmkES_server, but I'm worried it may do more than that.
add_subdirectory(../../am-cakeml "${CMAKE_CURRENT_BINARY_DIR}/am-cakeml")
