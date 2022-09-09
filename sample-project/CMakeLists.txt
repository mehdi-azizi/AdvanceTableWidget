#>>Cmake Generals
cmake_minimum_required(VERSION 3.10)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}
         VERSION 1.0
         LANGUAGES CXX)
#configure_file(${ProjectId}config.h.in ${ProjectId}Config.h)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)


#===== find Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTXML TRUE)

add_definitions(${QT_DEFINITIONS})

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)



#====== Adding project files and executable
file(GLOB ${ProjectId}_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp"
)
file(GLOB ${ProjectId}_HDR
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

file(GLOB ${ProjectId}_RES
     "${CMAKE_CURRENT_SOURCE_DIR}/res/*.qrc"
)
file(GLOB ${ProjectId}_UI
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../custom-table/include/
    ${CMAKE_CURRENT_BINARY_DIR}
    )


add_executable(${ProjectId}
               ${${ProjectId}_SRC}
               ${${ProjectId}_HDR}
               ${${ProjectId}_RES}
               ${${ProjectId}_UI}
     
               )



target_link_libraries(${ProjectId} PUBLIC
                       Qt5::Core
                       Qt5::Widgets
                       custom-table
                      )
















##========== install
#file(GLOB ${ProjectId}_INCLUDE
#     "${CMAKE_CURRENT_SOURCE_DIR}/data/*.h"
#)
#install(TARGETS ${ProjectId} DESTINATION "bin/${ProjectId}")
#install(FILES ${${ProjectId}_INCLUDE} DESTINATION include/${ProjectId}/data)

##========== uninstall
#add_custom_target("uninstall" COMMENT "Uninstall installed files")
#add_custom_command(
#    TARGET "uninstall"
#    POST_BUILD
#    COMMENT "Uninstall files with install_manifest.txt"
#    COMMAND xargs rm -vfr < install_manifest.txt || echo Nothing in
#            install_manifest.txt to be uninstalled!
#)
