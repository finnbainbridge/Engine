cmake_minimum_required(VERSION 3.10)

project(nuklear)

add_library(nuklear INTERFACE)
target_include_directories(nuklear INTERFACE .)
