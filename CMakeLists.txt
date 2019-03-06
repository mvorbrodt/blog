cmake_minimum_required(VERSION 3.13.4)

project(playground VERSION 0.1
	DESCRIPTION "Samples from http://blog.vorbrodt.me/"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

find_package(Boost)
#find_package (Boost COMPONENTS REQUIRED program_options wave atomic)
IF (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
	set (Boost_USE_STATIC_LIBS OFF)
	set (Boost_USE_MULTITHREAD ON)
endif()

find_package(TBB)
IF (TBB_FOUND)
	include_directories(${TBB_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=x86-64")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native")

add_executable(playground main.cpp)

target_link_libraries(playground ${Boost_LIBRARIES} ${TBB_LIBRARIES})
