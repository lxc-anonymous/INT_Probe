g++ -c raw.cpp -o raw.o
ar -rcs raw.a raw.o
# g++ 10-0-0-1.cpp raw.a -std=c++11

g++ -c udp.cpp -o udp.o
ar -rcs udp.a udp.o
# g++ 10-0-0-1.cpp udp.a -std=c++11
