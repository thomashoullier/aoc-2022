set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2 -Wextra -march=native")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(day4 ./day4.cpp)
