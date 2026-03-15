cmake_minimum_required(VERSION 3.15)
project(lob)

set(CMAKE_CXX_STANDARD 17)

# Main executable
add_executable(lob
    src/main.cpp
    src/order_book.cpp
)
target_include_directories(lob PRIVATE include)

# Benchmark executable
add_executable(lob_bench
    bench/bench_lob.cpp
    src/order_book.cpp
)
target_include_directories(lob_bench PRIVATE include)