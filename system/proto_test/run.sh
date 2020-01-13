protoc --cpp_out=. server_init.proto

g++ server.cpp -std=c++11 ../lib/udp.a -lprotobuf -lpthread server_init.pb.cc -o server
g++ controller.cpp -std=c++11 ../lib/udp.a -lprotobuf -lpthread server_init.pb.cc -o controller
