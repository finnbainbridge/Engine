cmake_minimum_required(VERSION 3.10)

project(termcolors VERSION 1.0)

add_library(termcolors INTERFACE)

target_include_directories(termcolors INTERFACE .)
