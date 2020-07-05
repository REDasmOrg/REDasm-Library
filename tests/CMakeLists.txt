cmake_minimum_required(VERSION 3.10)

project(Tests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB HEADERS *.h*)
file(GLOB SOURCES *.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} LibREDasm)
