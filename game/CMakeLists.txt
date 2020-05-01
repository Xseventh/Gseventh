project(Gseventh-Game)

list(APPEND LIBS ./reversi/ReversiEngine.cpp)

list(APPEND LIBS ./gomuku/GomukuEngine.cpp)

add_library(${PROJECT_NAME} STATIC ${LIBS})