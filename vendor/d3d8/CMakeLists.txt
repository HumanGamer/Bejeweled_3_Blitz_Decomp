project(d3d8lib)

add_library(d3d8lib INTERFACE d3d8.h d3d8caps.h d3d8types.h)

target_link_libraries(d3d8lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/d3d8.lib)
target_include_directories(d3d8lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
