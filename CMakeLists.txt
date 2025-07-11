cmake_minimum_required(VERSION 3.19)
project(CoilWhineBeGone LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

if(WIN32)
    set(APP_RESOURCE_FILE resources.rc)
endif()

qt_add_executable(CoilWhineBeGone
    WIN32 MACOSX_BUNDLE
    resources.rc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cpucycleburner.h
    cpucycleburner.cpp
)

qt_add_resources(CoilWhineBeGone "resources"
    PREFIX "/"
    FILES coilwhinebegone.ico
)

target_link_libraries(CoilWhineBeGone
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS CoilWhineBeGone
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CoilWhineBeGone
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
