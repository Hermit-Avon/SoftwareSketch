# 查找 pcap 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/cbf
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
)
link_directories(${PCAP_LIBRARY_DIRS})

add_executable(cbf
    main.cpp
)

target_link_libraries(cbf
    ${PCAP_LIBRARIES}
    pthread
)