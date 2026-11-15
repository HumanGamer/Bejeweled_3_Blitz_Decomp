project(PopPak)

add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME} PRIVATE
    PopPak.cpp
)

target_link_libraries(${PROJECT_NAME} Sexy::PakLib)
