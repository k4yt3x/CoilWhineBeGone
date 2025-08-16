cmake_minimum_required(VERSION 3.19)
project(CoilWhineBeGone LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(CoilWhineBeGone
    WIN32
    src/resources.rc
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/cpucycleburner.h
    src/cpucycleburner.cpp
    src/settingsmanager.h
    src/settingsmanager.cpp
)


target_compile_options(CoilWhineBeGone PRIVATE
    $<$<CONFIG:Release>:-Os>
)
target_link_options(CoilWhineBeGone PRIVATE
    $<$<CONFIG:Release>:-s>
)

qt_add_resources(CoilWhineBeGone "resources"
    PREFIX "/"
    FILES resources/coilwhinebegone.ico
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
