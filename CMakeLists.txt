cmake_minimum_required(VERSION 3.15)
project(colorsim_lib)

add_library(colorsim STATIC src/colorsim.hpp)

target_sources(colorsim PRIVATE
    src/colorsim.cpp
)

include_directories(PUBLIC SYSTEM ..)