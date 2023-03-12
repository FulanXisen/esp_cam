# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)


idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include"
    REQUIRES esp32-camera
    EMBED_FILES
        www/index_ov2640_html_gz
)
# register_component()
#target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_SOURCE_DIR}/lib")