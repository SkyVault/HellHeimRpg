cmake_minimum_required(VERSION 3.13)
project(Viakin)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
#set(CMAKE_CXX_FLAGS_DEBUG "-Og")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

include_directories(.)
include_directories(include/)

link_libraries(glfw GL GLEW assimp X11 dl pthread m)

add_executable(HellHeim.exe
	art.hpp
	art.cpp
	picker.hpp
	picker.cpp
	game.hpp
	game.cpp
	graphics.hpp
	graphics.cpp
	entt.hpp
        main.cpp)
