cmake_minimum_required(VERSION 3.13)
project(ex1_mtm C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic-errors -Werror")

link_directories(.)

add_executable(ex1_mtm mergeSort.c)

target_link_libraries(ex1_mtm libmtm.a)