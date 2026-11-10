project(SexyAppFramework)

add_library(${PROJECT_NAME} STATIC "")
target_sources(${PROJECT_NAME} PRIVATE
    SexyAppBase.cpp
    SexyAppBase.h
    SexyApp.cpp
    SexyApp.h
    Common.cpp
    Common.h
    BetaSupport.cpp
    BetaSupport.h
    InternetManager.cpp
    InternetManager.h
    HTTPTransfer.cpp
    HTTPTransfer.h
    Buffer.cpp
    Buffer.h
    WinInetHTTPTransfer.cpp
    WinInetHTTPTransfer.h
    ButtonListener.cpp
    ButtonListener.h
    DialogListener.cpp
    DialogListener.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Sexy::AppFramework ALIAS SexyAppFramework)