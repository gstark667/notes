cmake_minimum_required(VERSION 3.29)
project(KirigamiDemo LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets QuickControls2)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(BUILD_WITH_QT6 ON)

include(ECMFindQmlModule)
include(ECMQmlModule)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
)

add_subdirectory(QtWebDAV)

qt_add_executable(notes
    main.cpp
    Highlighter.cpp
    Syncer.cpp
    TreeModel.cpp
    TreeItem.cpp
    MANUAL_FINALIZATION
)

qt_add_qml_module(notes
    URI com.gstark
    VERSION 1.0
    QML_FILES
        qml/main.qml
    IMPORTS
        QtQuick.Controls
        org.kde.kirigami
)
qt_import_qml_plugins(notes)

target_link_libraries(notes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    KF6::Kirigami
    QtWebDAV
)

if(ANDROID)
    # add libomp and kirigami qml to the apk since qt doesn't bring it in automatically
    set_target_properties(notes PROPERTIES
        QT_ANDROID_EXTRA_LIBS
        ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/17/lib/linux/aarch64/libomp.so:/opt/kde/lib/qml/org/kde/kirigami/libqml_org_kde_kirigami_Kirigamiplugin_arm64-v8a.so
    )
    set_target_properties(notes PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
    )
    set_target_properties(notes PROPERTIES
        QT_ANDROID_PACKAGE_NAME "com.gstark.notes"
    )
endif()

qt_import_qml_plugins(notes)
qt_finalize_executable(notes)
