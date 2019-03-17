cmake_minimum_required(VERSION 3.0.0)

project(blog VERSION 0.1
	DESCRIPTION "Code samples from https://vorbrodt.blog"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
find_package(TBB REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization stacktrace_basic timer)

find_package(Protobuf REQUIRED)
find_package(Thrift REQUIRED)
find_package(GRPC REQUIRED)

find_package(MYSQL REQUIRED)
find_package(SOCI REQUIRED)

find_package(Catch2 REQUIRED)
find_package(Benchmark REQUIRED)
find_package(hayai REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native")
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(src)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
