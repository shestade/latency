cmake_minimum_required(VERSION 3.0.0)
project(latency VERSION 0.1.0)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -mavx2)

list(APPEND COMMON_HEADERS connectors.hpp rdtsc.hpp transport.hpp data.hpp launcher.hpp)

add_executable(sender sender.cpp ${COMMON_HEADERS})
set_target_properties(sender PROPERTIES CXX_STANDARD 20)

add_executable(receiver receiver.cpp ${COMMON_HEADERS})
set_target_properties(receiver PROPERTIES CXX_STANDARD 20)
