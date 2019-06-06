cmake_minimum_required(VERSION 3.12)
project(skeleton C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(skeleton
        exam.c
        exam.h
        main.c)
