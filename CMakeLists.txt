cmake_minimum_required(VERSION 3.13.4)

project(playground VERSION 0.1
	DESCRIPTION "Samples from http://blog.vorbrodt.me/"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

find_package(Boost REQUIRED)
#find_package (Boost COMPONENTS REQUIRED program_options)
find_package(TBB REQUIRED)
find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=x86-64")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native")

add_executable(playground
	main.cpp)

target_link_libraries(playground
	${Boost_LIBRARIES}
	${TBB_LIBRARIES}
	${Catch2_LIBRARIES}
	${GTest_LIBRARIES}
	${benchmark_LIBRARIES})
