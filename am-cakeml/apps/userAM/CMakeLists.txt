# NOTE: these config variables seem to be set too "late", leading to odd problems where the build only succeeds after 
# multiple re-configurations, unless the variables are set earlier
set(TARGET_ARCH "armv8" CACHE STRING "Options: \"native\", \"armv7\", \"armv8\", or \"other\"." FORCE)
# TODO: cake helper should see architecture and guess better
set(CAKE_FLAGS "--target=arm8 --stack_size=10 --heap_size=10" CACHE STRING "Arguments passed to the CakeML compiler" FORCE)

get_files(useram_src ${server_am_src} CrossVMDispatch.sml UserAM.sml)
build_posix_am(useram "${useram_src}")

get_files(client_src ${server_am_src} ClientTest.sml)
build_posix_am(client "${client_src}")
