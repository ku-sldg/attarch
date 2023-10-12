# Building the specialized file server component
# 9 October 2023
# Michael Neises

execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/embed_files.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE result_var
    OUTPUT_VARIABLE output_var
    ERROR_VARIABLE error_var
)

# Check if the script ran successfully
if(result_var)
    message(FATAL_ERROR "Error: ${error_var}")
else()
    message("Python script output: ${output_var}")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/FileSystem.c")
    message(FATAL_ERROR "FileSystem.c was not generated!")
endif()

DeclareCAmkESComponent(
    file_server    
    SOURCES
        file_server.c
        FileSystem.c
)

DeclareCAmkESComponent(
    tester_thinga    
    SOURCES
        thinga.c
)


