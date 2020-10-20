# Making my own cmake lists for tinyxml2 because their one kinda sucks
# It errors, and is overcomplicated
cmake_minimum_required(VERSION 3.10)

project(tinyxml2 VERSION 1.0)

add_library(tinyxml2 STATIC tinyxml2/tinyxml2.cpp)
target_include_directories(tinyxml2 PUBLIC ./tinyxml2)
