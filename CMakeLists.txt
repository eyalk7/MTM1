cmake_minimum_required(VERSION 3.13)
project(hw1_mtm C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic-errors -Werror ")

link_directories(.)

add_executable(ex1_mtm eurovision/eurovisionTestsMain.c eurovision/eurovisionTests.c eurovision/eurovision.c eurovision/functions.c eurovision/state.c eurovision/judge.c eurovision/map.c)

target_link_libraries(ex1_mtm libmtm.a)

