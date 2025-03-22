set(SRC_LIST callservice.cpp ../user.pb.cc)

add_executable(caller ${SRC_LIST})

target_link_libraries(caller protobuf protoc mprpc)