set(TARGET_ARCH "armv8" CACHE STRING "Options: \"native\", \"armv7\", \"armv8\", or \"other\"." FORCE)
set(CAKE_FLAGS "--target=arm8 --stack_size=10 --heap_size=10" CACHE STRING "Arguments passed to the CakeML compiler" FORCE)
set(STATIC_LINKING on CACHE BOOL "" FORCE)