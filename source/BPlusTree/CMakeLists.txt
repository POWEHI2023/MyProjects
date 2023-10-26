project(BPlusTree)

file(GLOB c_files *.cpp *.c)

add_library(add_bplib_shared SHARED ${c_files})

set_target_properties(add_bplib_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(add_bplib_shared PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS add_bplib_shared LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib)
