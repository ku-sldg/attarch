#get_files(useram_src ${server_am_src} CrossVMDispatch.sml UserAM.sml)
#build_posix_am(useram "${useram_src}")

#get_files(client_src ${server_am_src} ClientTest.sml)
#build_posix_am(useramClientTest "${client_src}")
