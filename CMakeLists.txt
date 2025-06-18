cmake_minimum_required(VERSION 3.10)
project(countmin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 include 目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src/countmin
    ${CMAKE_SOURCE_DIR}/include
)

# 查找 pcap 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

include_directories(${PCAP_INCLUDE_DIRS})
link_directories(${PCAP_LIBRARY_DIRS})

# 添加可执行文件
add_executable(countmin
    src/countmin/countmin.cpp
)

# 链接库
target_link_libraries(countmin
    ${PCAP_LIBRARIES}
    pthread
)

# 如果有 xxhash 源文件
if(EXISTS "${CMAKE_SOURCE_DIR}/include/xxhash.c")
    target_sources(countmin PRIVATE ${CMAKE_SOURCE_DIR}/include/xxhash.c)
endif()

# 如果有 protocol_parsing 源文件
if(EXISTS "${CMAKE_SOURCE_DIR}/include/protocol_parsing.c")
    target_sources(countmin PRIVATE ${CMAKE_SOURCE_DIR}/include/protocol_parsing.c)
endif()