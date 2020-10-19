project('tinyxml2', 'c', 'cpp')

tinyxml2_inc = include_directories('tinyxml2/tinyxml2')
tinyxml2_lib = static_library('tinyxml2', 'tinyxml2/tinyxml2.cpp')
tinyxml2 = declare_dependency(link_with : tinyxml2_lib, include_directories : tinyxml2_inc)