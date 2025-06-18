cmake_minimum_required(VERSION 3.10)
project(SoftwareSketch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器为 clang++
set(CMAKE_CXX_COMPILER "clang++")

# 递归添加 src 目录
add_subdirectory(src)