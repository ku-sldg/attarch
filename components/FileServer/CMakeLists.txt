# Building the specialized file server component
# 9 October 2023
# Michael Neises

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


