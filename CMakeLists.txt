cmake_minimum_required(VERSION 3.15)
project(colorsim_lib)

add_library(colorsim STATIC src/colorsim.hpp)

target_sources(colorsim PRIVATE
    src/colorsim.cpp
)

set_target_properties(colorsim PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

include_directories(PUBLIC SYSTEM ..)