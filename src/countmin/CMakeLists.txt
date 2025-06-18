# 查找 pcap 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/countmin
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
)
link_directories(${PCAP_LIBRARY_DIRS})

add_executable(countmin
    countmin.cpp
)

target_link_libraries(countmin
    ${PCAP_LIBRARIES}
    pthread
)

if(EXISTS "${CMAKE_SOURCE_DIR}/include/xxhash.c")
    target_sources(countmin PRIVATE ${CMAKE_SOURCE_DIR}/include/xxhash.c)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/include/protocol_parsing.c")
    target_sources(countmin PRIVATE ${CMAKE_SOURCE_DIR}/include/protocol_parsing.c)
endif()