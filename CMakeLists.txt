cmake_minimum_required(VERSION 3.1...3.13)
  
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(playground VERSION 0.0.1
	DESCRIPTION "Samples from http://blog.vorbrodt.me/"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -march=x86-64")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native")

add_executable(playground main.cpp)

target_link_libraries(playground tbb)

set_property(TARGET playground PROPERTY CXX_STANDARD 17)
