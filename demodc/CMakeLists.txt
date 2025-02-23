cmake_minimum_required(VERSION 3.29)

# Rules for building the demo.  Doesn't use an installed gme if you've already
# installed the project so if you're copying these rules you probably don't
# even need these next two lines if you're building against system-installed
# gme.
include_directories(${CMAKE_SOURCE_DIR}/gme ${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_BINARY_DIR}/gme)

add_executable(demo example.c)

# Add command to copy build file over.
add_custom_command(TARGET demo
    POST_BUILD
    COMMAND cmake -E copy "${CMAKE_SOURCE_DIR}/test.nsf" ${CMAKE_CURRENT_SOURCE_DIR}/romdisk/test.nsf
    COMMENT "Add convenience copy of test.nsf file for demo application"
    VERBATIM) # VERBATIM is essentially required, "please use correct command line kthx"

target_link_libraries(demo gme)

if (OS_KOS) # Dreamcast
    target_link_libraries(demo kallisti)
	# Add romdisk
    set(KOS_ROMDISK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/romdisk")
    kos_add_romdisk(demo ${KOS_ROMDISK_DIR})
endif()