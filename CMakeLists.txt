cmake_minimum_required(VERSION 3.23)
project(CN_Project C)

set(CMAKE_C_STANDARD 17)

add_executable(client main.c Client.c Client.h)

target_link_libraries(client -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf)
if(WIN32)
    target_link_libraries(client wsock32 ws2_32)
endif()

add_executable(server Server.c)
if(WIN32)
    target_link_libraries(server wsock32 ws2_32)
endif()