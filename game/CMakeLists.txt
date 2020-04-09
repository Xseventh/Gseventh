project(Gseventh-Game)

list(APPEND LIBS ./reversi/ReversiEngine.cpp)

add_library(${PROJECT_NAME} STATIC ${LIBS})