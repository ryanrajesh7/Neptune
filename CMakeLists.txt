cmake_minimum_required(VERSION 3.15)
project(NEPTUNE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(neptune src/main.cpp src/interface.cpp src/engine.cpp src/board.cpp)

if(MSVC)
    target_compile_options(neptune PRIVATE /O2 /Oi /Ot /arch:AVX2)
else()
    target_compile_options(neptune PRIVATE -O3 -march=native -flto)
    target_link_options(neptune PRIVATE -flto) 
endif()