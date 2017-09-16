

project(PNGreader)

cmake_minimum_required(VERSION 2.8)
include_directories(include)
aux_source_directory(src DIR_SRCS)
add_executable(read ${DIR_SRCS})
