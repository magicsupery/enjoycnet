cmake_minimum_required(VERSION 3.2)
set(projectName enjoycnet)

project(${projectName})

# build config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS "Build shared libs" OFF)


if (CMAKE_BUILD_TYPE)
else()
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()


if (UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++17 -fPIC -Wall -m64 ${CMAKE_CXX_FLAGS}")
	#set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG}")
	#set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (WIN32)
    # windows platform
    #add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /EHsc")
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("  CMAKE_COMMAND: ${CMAKE_COMMAND}")
message("  CMAME_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("  CMAKE_PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("----------------------------------")

# find boost
set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost COMPONENTS system context)
if(Boost_FOUND)
else()
	message("can not find boost, please install boost first")
	EXIT()	
endif()

#include
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/include)

#lib
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/lib)
link_directories(${PROJECT_BINARY_DIR}/lib)
link_directories(${Boost_LIBRARY_DIRS})



add_subdirectory(enjoycnet)
add_subdirectory(test)


