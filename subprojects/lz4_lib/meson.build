project('lz4', 'c')

lz4_inc = include_directories('lz4/lib')
lz4_lib = static_library('lz4', 'lz4/lib/lz4.c')
lz4 = declare_dependency(link_with : lz4_lib, include_directories : lz4_inc)