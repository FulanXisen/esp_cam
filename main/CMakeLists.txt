# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE srcs 
    ${CMAKE_SOURCE_DIR}/main/*.c
)

MESSAGE( STATUS "SRCS: ${srcs}")
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS 
        "${CMAKE_SOURCE_DIR}/include" 
        "${CMAKE_SOURCE_DIR}/main"
        "${CMAKE_SOURCE_DIR}/main/module/m_camera"
    #REQUIRES esp_camera
    EMBED_FILES
        www/index_ov2640_html_gz
)
