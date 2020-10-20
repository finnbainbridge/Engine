cmake_minimum_required(VERSION 3.10)
project(lz4)

add_library(lz4 STATIC lz4/lib/lz4.c)

target_include_directories(lz4 PUBLIC lz4/lib)
