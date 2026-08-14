cmake_minimum_required(VERSION 3.19)
project(VE2REHConfig LANGUAGES CXX)

include(FetchContent)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(VE2REHConfig
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sitesdialog.h 
    sitesdialog.cpp 
    sitesdialog.ui
    resources.qrc
)

# Add resources.rc for Windows builds
if(WIN32)
    target_sources(VE2REHConfig PRIVATE resources.rc)
endif()

FetchContent_Declare(
    qtkeychain
    GIT_REPOSITORY https://github.com/frankosterfeld/qtkeychain.git
    GIT_TAG 0.17
)

set(BUILD_WITH_QT6 ON CACHE BOOL "Build qtkeychain with Qt6" FORCE)
set(BUILD_TEST_APPLICATION OFF CACHE BOOL "Disable QtKeychain test app" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable QtKeychain tests" FORCE)
FetchContent_MakeAvailable(qtkeychain)

target_link_libraries(VE2REHConfig
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        qt6keychain
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
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sites_config.json
    ${CMAKE_CURRENT_BINARY_DIR}/sties_config.json
    COPYONLY
)
