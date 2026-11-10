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
    SEHCatcher.cpp
    SEHCatcher.h
    SharedImage.cpp
    SharedImage.h
    CritSect.cpp
    CritSect.h
    AutoCrit.h
    Point.h
    Rect.h
    Ratio.cpp
    Ratio.h
    WidgetContainer.cpp
    WidgetContainer.h
    ImageFont.cpp
    ImageFont.h
    PIEffect.cpp
    PIEffect.h
    PopAnim.cpp
    PopAnim.h
    PopLoc.cpp
    PopLoc.h
    SharedRenderTarget.cpp
    SharedRenderTarget.h
    CfgMachine.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC Sexy::ImageLib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_USE_WIDE_STRING)

add_library(Sexy::AppFramework ALIAS SexyAppFramework)