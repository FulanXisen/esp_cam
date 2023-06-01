# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

MESSAGE( STATUS "SRCS: ${app_sources}")
idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include"
    #REQUIRES esp_camera
    EMBED_FILES
        www/index_ov2640_html_gz
)

#register_component()
#target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_SOURCE_DIR}/lib")