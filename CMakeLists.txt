cmake_minimum_required(VERSION 3.19)
project(VE2REHConfig LANGUAGES CXX)

include(FetchContent)


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(VE2REHConfig
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
        sitesdialog.h 
	sitesdialog.cpp 
	sitesdialog.ui

)

FetchContent_Declare(
    qtkeychain
    GIT_REPOSITORY https://github.com/frankosterfeld/qtkeychain.git
    GIT_TAG main
)

set(BUILD_WITH_QT6 ON CACHE BOOL "Build qtkeychain with Qt6" FORCE)

#FetchContent_MakeAvailable(qtkeychain)

add_subdirectory(third_party/qtkeychain)

target_link_libraries(VE2REHConfig
    PRIVATE
        Qt::Core
        Qt::Widgets
	Qt::Network
        qtkeychain
)

include(GNUInstallDirs)

install(TARGETS VE2REHConfig
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET VE2REHConfig
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
