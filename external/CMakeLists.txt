project(Gseventh-External)

list(APPEND LIBS ./jsoncpp/jsoncpp.cpp)

add_library(${PROJECT_NAME} STATIC ${LIBS})