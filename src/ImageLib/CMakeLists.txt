project(ImageLib)

add_library(${PROJECT_NAME} STATIC "")
target_sources(${PROJECT_NAME} PRIVATE
    ImageLib.cpp
    ImageLib.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Sexy::ImageLib ALIAS ImageLib)
