cmake_minimum_required(VERSION 3.15)
project(lob)

set(CMAKE_CXX_STANDARD 17)

# Main executable
add_executable(lob
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/order_book.cpp
    ${CMAKE_SOURCE_DIR}/src/order_pool.cpp
)
target_include_directories(lob PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Benchmark executable
add_executable(lob_bench
    ${CMAKE_SOURCE_DIR}/bench/bench_lob.cpp
    ${CMAKE_SOURCE_DIR}/src/order_book.cpp
    ${CMAKE_SOURCE_DIR}/src/order_pool.cpp
)
target_include_directories(lob_bench PRIVATE ${CMAKE_SOURCE_DIR}/include)