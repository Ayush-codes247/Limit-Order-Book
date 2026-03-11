cmake_minimum_required(VERSION 3.15)
project(lob)

set(CMAKE_CXX_STANDARD 17)

add_executable(lob
    src/main.cpp
    src/order_book.cpp
)

# Tell CMake where to find .h files
target_include_directories(lob PRIVATE include)