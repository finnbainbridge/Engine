cmake_minimum_required(VERSION 3.1.0)
project(Engine VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

add_library(Engine OBJECT include/glad/glad.h lib/glad/src/glad.c include/Engine/Engine.hpp include/Engine/Renderer/Renderer.hpp include/Engine/Renderer/Amber.hpp
            include/Engine/Element3D.hpp
             src/res.cpp src/engine.cpp src/DOM/dom.cpp src/threading.cpp src/renderer/Amber/amber.cpp
             src/Element3D/element3d.cpp
            )

target_include_directories(Engine PUBLIC include)
include_directories(include)

find_package(Threads REQUIRED)
target_link_libraries(Engine PRIVATE Threads::Threads)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw)

target_link_libraries(Engine PRIVATE glfw)
target_link_libraries(Engine PRIVATE GL)
target_link_libraries(Engine PRIVATE dl)


function(make_test test_name source)
    set (CMAKE_CXX_STANDARD 17)
    add_executable(${test_name} ${source} $<TARGET_OBJECTS:Engine>)
    add_test(NAME ${test_name} COMMAND ${test_name})
    find_package(Threads REQUIRED)
    target_link_libraries(${test_name} PRIVATE Threads::Threads)
    target_link_libraries(${test_name} PRIVATE glfw)
    target_link_libraries(${test_name} PRIVATE GL)
    target_link_libraries(${test_name} PRIVATE dl)
endfunction(make_test)

make_test(test1 tests/test1.cpp)
make_test(IdGetTest tests/IdGetTest.cpp)
make_test(RunTest tests/RunTest.cpp)

make_test(RenderTest tests/RenderTest.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
