project(Bejeweled3)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 "")
else()
    add_executable(${PROJECT_NAME} "")
endif()

target_sources(${PROJECT_NAME} PRIVATE
    Bej3.cpp
    Bej3App.cpp
    Bej3App.h
)

target_link_libraries(${PROJECT_NAME} Sexy::AppFramework)
