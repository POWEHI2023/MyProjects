
project(BinarySearchTree)

file(GLOB c_files *.cpp *.c)
file(GLOB h_files *.h)

add_library(add_bstlib_shared SHARED ${c_files})
add_library(add_bstlib_static STATIC ${c_files})

# foreach(file ${f_files})
#   install(FILES files DESTINATION include) 
# endforeach()
# install(TARGETS add_bstlib_shared LIBRARY DESTINATION lib)
# install(TARGETS add_bstlib_static ARCHIVE DESTINATION lib)

set_target_properties(add_bstlib_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(add_bstlib_shared PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(add_bstlib_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(add_bstlib_static PROPERTIES VERSION 1.0 SOVERSION 1)

# install(TARGETS add_bstlib_shared LIBRARY DESTINATION lib)