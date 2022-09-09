#>>Cmake Generals
cmake_minimum_required(VERSION 3.10)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}
         VERSION 1.0
         LANGUAGES CXX)

######################################
#   Projecy sub dirs
######################################
add_subdirectory(custom-table)
add_subdirectory(sample-project)

