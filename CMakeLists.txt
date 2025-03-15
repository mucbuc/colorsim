cmake_minimum_required(VERSION 3.15)
project(ColorSimLib)

target_sources(ColorSimLib 
    INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/colorsim.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/colorsim.cpp"
)

