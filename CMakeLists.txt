cmake_minimum_required(VERSION 3.14)
project(CPP_Projects)

# set(CMAKE_C_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()





#
# SOME COMPILER CONFIG
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wunused-command-line-argument")
string(TIMESTAMP COMPILE_TIME %Y%m%d-%H%M%S)

message(STATUS "build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX_FLAGS = ${CMAKE_CXX_FLAGS} ")





#
# SOME PATH SETTING
#
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib)
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib)
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/bin)
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin")





# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )

# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest) 

# enable_testing()

# include(GoogleTest)




#
# ADD INCLUDE DIRECTORY
#
function(include_directories_recursively root_dir)
  if(IS_DIRECTORY ${root_dir})
    message("include dir: " ${root_dir})
    include_directories(${root_dir})
  endif()

  file(GLOB ALL_SUB RELATIVE ${root_dir} ${root_dir}/*)
  foreach(sub ${ALL_SUB})

    if(IS_DIRECTORY ${root_dir}/${sub})
      include_directories_recursively(${root_dir}/${sub})
    endif()

  endforeach()

endfunction()
message("include all sub directoried recursively ")
include_directories_recursively(${PROJECT_SOURCE_DIR}/source)
add_subdirectory(source)




#
# LINK AND GENERATE EXECUTABLE
#
link_directories(${LIBRARY_OUTPUT_PATH})
# execute_process(COMMAND pwd)
# file(GLOB_RECURSE c_files *.cpp *.c)
# link_directories(${PROJECT_BINARY_DIR}/lib)
# add_executable(ProjectMain main.cpp)
# target_link_libraries(ProjectMain add_bplib_static add_bstlib_static)




#
# PREPARE FORE INSTALLING
#
function(CheckAndCreateDir dir)
  if(NOT EXISTS ${dir})
    execute_process(COMMAND mkdir ${dir})
    message(STATUS "create dir ${dir}")
  else()
    message(STATUS "find dir ${dir}")
  endif()
endfunction()

function(CheckAndCreateFile file)
  if(NOT EXISTS ${file})
    execute_process(COMMAND touch ${file})
    message(STATUS "create file ${file}")
  endif()
endfunction()

CheckAndCreateDir(${CMAKE_SOURCE_DIR}/output/bin)
CheckAndCreateDir(${CMAKE_SOURCE_DIR}/output/lib)
CheckAndCreateDir(${CMAKE_SOURCE_DIR}/output/include)
# install(TARGETS ProjectMain RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)