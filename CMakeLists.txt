cmake_minimum_required(VERSION 2.8)
project(ODirect)

add_library(o_direct SHARED o_direct.c)
target_link_libraries(o_direct dl)
