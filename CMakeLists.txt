cmake_minimum_required(VERSION 3.0.0)
project(Engine VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(Engine OBJECT include/Engine/Engine.hpp include/Engine/DOM.hpp src/engine.cpp src/DOM/dom.cpp)

target_include_directories(Engine PUBLIC include)
include_directories(include)

add_executable(test1 tests/test1.cpp $<TARGET_OBJECTS:Engine>)
add_test(NAME test1 COMMAND test1)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
