add_executable(playground main.cpp)

add_executable(database database.cpp)
set_source_files_properties(database.cpp PROPERTIES COMPILE_FLAGS -std=c++11)
target_include_directories(database PUBLIC ${SOCI_INCLUDE_DIRS})
target_link_libraries(database ${SOCI_LIBRARY})

add_executable(stacktrace stacktrace.cpp)
target_link_libraries(stacktrace ${CMAKE_DL_LIBS} ${Boost_STACKTRACE_LIBRARY})

add_executable(uuid uuid.cpp)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.pb.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.pb.cc
	COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}
	--cpp_out=${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.proto)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.grpc.pb.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.grpc.pb.cc
	COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}
	--grpc_out=${CMAKE_CURRENT_SOURCE_DIR}
	--plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
	${CMAKE_CURRENT_SOURCE_DIR}/grpc_service.proto)
add_executable(grpc_service_c grpc_service_c.cpp grpc_service.pb.cc grpc_service.grpc.pb.cc)
target_link_libraries(grpc_service_c ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})
add_executable(grpc_service_s grpc_service_s.cpp grpc_service.pb.cc grpc_service.grpc.pb.cc)
target_link_libraries(grpc_service_s ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(xml xml.cpp)
target_link_libraries(xml ${Boost_SERIALIZATION_LIBRARY})

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ThriftService.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ThriftService.cpp
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/ThriftService_server.skeleton.cpp
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/thrift_service_constants.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/thrift_service_constants.cpp
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/thrift_service_types.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/thrift_service_types.cpp
	COMMAND thrift -out ${CMAKE_CURRENT_SOURCE_DIR}
	-gen cpp ${CMAKE_CURRENT_SOURCE_DIR}/thrift_service.thrift)
add_executable(thrift_service_c thrift_service_c.cpp ThriftService.cpp thrift_service_constants.cpp thrift_service_types.cpp)
target_link_libraries(thrift_service_c ${THRIFT_LIBRARIES})
add_executable(thrift_service_s thrift_service_s.cpp ThriftService.cpp thrift_service_constants.cpp thrift_service_types.cpp)
target_link_libraries(thrift_service_s ${THRIFT_LIBRARIES})

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/protobuf.pb.h
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/protobuf.pb.cc
	COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR}
	--cpp_out=${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/protobuf.proto)
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
