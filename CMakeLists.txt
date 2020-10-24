cmake_minimum_required(VERSION 3.10)

project(Engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(Engine STATIC  src/engine.cpp   
         src/res.cpp  
         src/threading.cpp  
         src/DevTools/devtoolsui.cpp  
         src/DevTools/log.cpp  
         src/DevTools/orbitcam.cpp  
         src/DOM/dom.cpp  
         src/Element3D/element3d.cpp  
         src/Element3D/meshelement3d.cpp  
         src/Element3D/models.cpp  
         src/renderer/Amber/amber.cpp
         src/renderer/Amber/amber_resources.cpp)
        
target_include_directories(Engine PUBLIC include)
include_directories(include)

# Enable Hell
# Don't: TOO MANY WARNINGS!
# target_compile_options(Engine PRIVATE
#   $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
#   $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -pedantic -Werror>
# )

find_package(Threads REQUIRED)
target_link_libraries(Engine PUBLIC Threads::Threads)


if (NOT EMSCRIPTEN)
    message("Building for Desktop (OpenGL ES 3.0)")
    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(subprojects/glfw)
    
    # GLAD
    add_subdirectory(subprojects/glad)
    
    target_link_libraries(Engine PUBLIC glad)

    if (NOT WIN32)
        target_link_libraries(Engine PUBLIC dl)
    endif()
endif()

# if (EMSCRIPTEN)
#     message("Building for Web (Emscripten/WebGL 2)")
#     set(CMAKE_EXECUTABLE_SUFFIX ".html")

#     # TODO: change this
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file \"${ENGINE_PACKAGE_FILES}\"")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -s USE_WEBGL2=1 -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s LLD_REPORT_UNDEFINED")

#     set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s LLD_REPORT_UNDEFINED -lglfw")
# endif()

function(build_emscripten)
    if (EMSCRIPTEN)
        message("Building for Web (Emscripten/WebGL 2)")
        set(CMAKE_EXECUTABLE_SUFFIX ".html")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENGINE_PACKAGE_FILES}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -s USE_WEBGL2=1 -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s LLD_REPORT_UNDEFINED")

        set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s LLD_REPORT_UNDEFINED -lglfw")
    endif()
endfunction(build_emscripten)


# Link glfw and gl
if (WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Engine PUBLIC OpenGL::GL)
else()
    target_link_libraries(Engine PUBLIC GL)
endif()

target_link_libraries(Engine PUBLIC glfw)
# target_link_libraries(Engine PUBLIC GL)

target_link_libraries(Engine PUBLIC ${GLFW_LIBRARIES})

# Add other dependencies

# GLM
add_subdirectory(subprojects/glm_lib)
target_link_libraries(Engine PUBLIC glm)

# LZ4
add_subdirectory(subprojects/lz4_lib)
target_link_libraries(Engine PUBLIC lz4)

# Nuklear
add_subdirectory(subprojects/nuklear)
target_link_libraries(Engine PUBLIC nuklear)

# Term-colors
add_subdirectory(subprojects/termcolors_lib)
target_link_libraries(Engine PUBLIC termcolors)

# Tinyxml2
add_subdirectory(subprojects/tinyxml2_lib)
target_link_libraries(Engine PUBLIC tinyxml2)

