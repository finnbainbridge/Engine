project('Engine', 'c', 'cpp', default_options : ['c_std=c11', 'cpp_std=c++17','warning_level=0'])
warning_level = 0


# Dependencies
thread_dep = dependency('threads')

# Cmake dependencies
cmake = import('cmake')

# GLFW
glfw_sub = cmake.subproject('glfw')
glfw = glfw_sub.dependency('glfw')

# Meson dependencies

# Glad
glad_sub = subproject('glad')
glad = glad_sub.get_variable('glad')

# GLM
glm_sub = subproject('glm_lib')
glm = glm_sub.get_variable('glm')

# KHR
khr_inc = include_directories('subprojects/KHR')
khr = declare_dependency(include_directories: khr_inc)

# LZ4
lz4_sub = subproject('lz4_lib')
lz4 = lz4_sub.get_variable('lz4')

# Tinyxml2
tinyxml2_sub = subproject('tinyxml2_lib')
tinyxml2 = tinyxml2_sub.get_variable('tinyxml2')

# Nuklear
nuklear_inc = include_directories('subprojects/nuklear')
nuklear = declare_dependency(include_directories: nuklear_inc)

# Term-colors
termcolors_sub = subproject('termcolors_lib')
termcolors = termcolors_sub.get_variable('termcolors')

# Source code
src = ['src/engine.cpp', 
        'src/res.cpp',
        'src/threading.cpp',
        'src/DevTools/devtoolsui.cpp',
        'src/DevTools/log.cpp',
        'src/DevTools/orbitcam.cpp',
        'src/DOM/dom.cpp',
        'src/Element3D/element3d.cpp',
        'src/Element3D/meshelement3d.cpp',
        'src/Element3D/models.cpp',
        'src/renderer/Amber/amber.cpp']

# Include
engine_include = include_directories('include')

engine_lib = library('Engine', src, version: '0.0.1', 
            include_directories: engine_include,
            dependencies: [thread_dep,
                            glfw,
                            glm,
                            glad,
                            khr,
                            lz4,
                            tinyxml2,
                            termcolors,
                            nuklear],
            override_options : ['c_std=c11'])

engine = declare_dependency(link_with : engine_lib, include_directories : engine_include)