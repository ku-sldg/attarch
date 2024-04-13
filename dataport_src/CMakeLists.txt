cmake_minimum_required(VERSION 3.10)
project(dataport-app)

# Add the executable
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(dataportAPI main.c dataport_interface.c)

