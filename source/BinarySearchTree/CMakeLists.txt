project(BinarySearchTree)

file(GLOB c_files bst.cpp)

add_library(add_bstlib_shared SHARED ${c_files})

set_target_properties(add_bstlib_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(add_bstlib_shared PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS add_bstlib_shared LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib)