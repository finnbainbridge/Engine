project('glad', 'c')

glad_inc = include_directories('include')

glad_lib = static_library('glad', 'src/glad.c')

glad = declare_dependency(link_with: glad_lib, include_directories: glad_inc)