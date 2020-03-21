cmake_minimum_required(VERSION 3.14)
project(Gseventh)

set(CMAKE_CXX_STANDARD 11)


set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Gseventh main.cpp algorithm/max-min/alpha-beta.h game/reversi_alphabeta.cpp game/reversi_alphabeta.h common/bitset.h common/k_dimensional_array.h common/template.h common/scanner.h)