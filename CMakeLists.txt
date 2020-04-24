cmake_minimum_required(VERSION 3.14)
project(Gseventh)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}
)

add_subdirectory(./game)
add_subdirectory(./external)

add_executable(Gseventh main.cpp)

set(CMAKE_EXE_LINKER_FLAGS "-static")

target_link_libraries(${PROJECT_NAME} Gseventh-Game)
