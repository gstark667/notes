cmake_minimum_required(VERSION 3.19)
project(KirigamiDemo LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets QuickControls2)


find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMFindQmlModule)
include(ECMQmlModule)

if(ANDROID)
add_subdirectory(kirigami)
else()
ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
)
endif()

qt_add_executable(notes
    main.cpp
    Highlighter.cpp
    TreeModel.cpp
    TreeItem.cpp
    MANUAL_FINALIZATION
)

qt_add_qml_module(notes
    URI notes
    VERSION 1.0
    QML_FILES
        qml/main.qml
    IMPORTS
        QtQuick.Controls 2.15
        org.kde.kirigami 2.20
)

target_link_libraries(notes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Kirigami
)

if(ANDROID)
    # add libomp to the apk since qt doesn't bring it in automatically
    set_target_properties(notes PROPERTIES
        QT_ANDROID_EXTRA_LIBS
        ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/20/lib/linux/aarch64/libomp.so
    )
    set_target_properties(notes PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
    )
endif()

qt_import_qml_plugins(notes)
qt_finalize_executable(notes)
