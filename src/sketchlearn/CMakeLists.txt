find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/sketchlearn
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
)
link_directories(${PCAP_LIBRARY_DIRS})

add_executable(sketchlearn
    main.cpp
    twotuple.cpp
)

target_link_libraries(sketchlearn
    ${PCAP_LIBRARIES}
    pthread
)