

project(PNGreader)

if(DEFINED $ENV{MSYSTEM})
    set(CMAKE_C_COMPILER "/mingw64/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "/mingw64/bin/g++.exe")
endif()

cmake_minimum_required(VERSION 2.8)
include_directories(include)
aux_source_directory(src DIR_SRCS)
add_executable(read ${DIR_SRCS})
