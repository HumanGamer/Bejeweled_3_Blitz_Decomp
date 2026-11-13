project(PakLib)

add_library(${PROJECT_NAME} STATIC "")
target_sources(${PROJECT_NAME} PRIVATE
    PakInterface.cpp
    PakInterface.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Sexy::PakLib ALIAS PakLib)
