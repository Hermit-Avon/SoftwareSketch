cmake_minimum_required(VERSION 3.10)
project(SoftwareSketch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 递归添加 src 目录
add_subdirectory(src)