
project(Source)

message(STATUS "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB ALL_SUB RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(sub ${ALL_SUB})
  set(CRT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${sub})
  if(IS_DIRECTORY ${CRT_DIR})
    message(STATUS "add subdirectory ${sub}")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${sub})
  endif()
endforeach()

file(GLOB_RECURSE h_files *.h)

foreach(f ${h_files})
  install(FILES ${f} DESTINATION ${CMAKE_SOURCE_DIR}/include/)
endforeach()