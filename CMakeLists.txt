cmake_minimum_required(VERSION 3.14)
project(Gseventh)

set(CMAKE_CXX_STANDARD 14)

INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}
)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Gseventh main.cpp algorithm/max-min/AlphaBetaEngine.h common/bitset.h common/k_dimensional_array.h common/template.h common/scanner.h algorithm/mcts/MCTSEngine.h common/singleton.h game/reversi/ReversiEngine.cpp game/reversi/ReversiEngine.h)