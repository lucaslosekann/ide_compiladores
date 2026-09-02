cmake_minimum_required(VERSION 3.19)
project(ide_compiladores LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(ide_compiladores
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    settingsdialog.h settingsdialog.cpp settingsdialog.ui
)

qt_add_translations(
    TARGETS ide_compiladores
    TS_FILES ide_compiladores_pt_BR.ts
)

target_link_libraries(ide_compiladores
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS ide_compiladores
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ide_compiladores
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
