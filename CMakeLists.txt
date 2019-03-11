cmake_minimum_required(VERSION 3.13.4)

project(blog VERSION 0.1
	DESCRIPTION "Code samples from https://vorbrodt.blog"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

find_package(TBB REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Thrift REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=x86-64")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native")

add_executable(playground main.cpp)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/ThriftService.h
	OUTPUT ${PROJECT_SOURCE_DIR}/ThriftService.cpp
	OUTPUT ${PROJECT_SOURCE_DIR}/ThriftService_server.skeleton.cpp
	OUTPUT ${PROJECT_SOURCE_DIR}/thrift_service_constants.h
	OUTPUT ${PROJECT_SOURCE_DIR}/thrift_service_constants.cpp
	OUTPUT ${PROJECT_SOURCE_DIR}/thrift_service_types.h
	OUTPUT ${PROJECT_SOURCE_DIR}/thrift_service_types.cpp
	COMMAND thrift -out ${PROJECT_SOURCE_DIR}
	-gen cpp ${PROJECT_SOURCE_DIR}/thrift_service.thrift)
add_executable(thrift_service_c thrift_service_c.cpp ThriftService.cpp thrift_service_constants.cpp thrift_service_types.cpp)
target_link_libraries(thrift_service_c ${THRIFT_LIBRARIES})
add_executable(thrift_service_s thrift_service_s.cpp ThriftService.cpp thrift_service_constants.cpp thrift_service_types.cpp)
target_link_libraries(thrift_service_s ${THRIFT_LIBRARIES})

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/protobuf.pb.h
	OUTPUT ${PROJECT_SOURCE_DIR}/protobuf.pb.cc
	COMMAND protoc -I=${PROJECT_SOURCE_DIR}
	--cpp_out=${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/protobuf.proto)
add_executable(protobuf protobuf.cpp protobuf.pb.cc)
target_link_libraries(protobuf ${Protobuf_LIBRARIES})

add_executable(pstl pstl_test.cpp)
target_link_libraries(pstl ${TBB_LIBRARIES})

add_executable(timer timer_test.cpp)
add_executable(pool pool_test.cpp)
add_executable(random random_test.cpp)
add_executable(dekker_n dekker_n.cpp)
add_executable(dekker dekker.cpp)
add_executable(membars membars.cpp)
add_executable(atomics atomics.cpp)
add_executable(unordered unordered.cpp)
add_executable(templates templates.cpp)
add_executable(interview_6 interview_6.cpp)
add_executable(inverting inverting.cpp)
add_executable(interview_5 interview_5.cpp)
add_executable(interview_4 interview_4.cpp)
add_executable(interview_2 interview_2.cpp)
add_executable(interview_1 interview_1.cpp)
add_executable(class class.cpp)
add_executable(queue queue.cpp)
add_executable(cache cache.cpp)
add_executable(sorting sorting.cpp)
add_executable(aos_soa aos_soa.cpp)
add_executable(new new.cpp)
add_executable(enum2 enum2.cpp)
add_executable(enum enum.cpp)
