cmake_minimum_required(VERSION 3.19)
project(KirigamiDemo LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)


find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMFindQmlModule)
include(ECMQmlModule)

if(ANDROID)
add_subdirectory(kirigami)
endif()

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.delegates REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.settings REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.treeview REQUIRED)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    CoreAddons
    Config
    ItemModels
    KirigamiAddons
)

find_package(KF6 REQUIRED COMPONENTS KirigamiAddons)

qt_add_executable(notes
    main.cpp
    FileModel.cpp
    TreeModel.cpp
    TreeItem.cpp
    MANUAL_FINALIZATION
)

qt_add_qml_module(notes
    URI KirigamiDemo
    VERSION 1.0
    QML_FILES
        main.qml
)

qt_add_resources(notes "resources"
    PREFIX "/"
    FILES
        main.qml
)

target_link_libraries(notes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    
    KF6::Kirigami
    KF6::ItemModels
)

if(ANDROID)
    # add libomp to the apk since qt doesn't bring it in automatically
    set_target_properties(notes PROPERTIES 
        QT_ANDROID_EXTRA_LIBS
        ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/20/lib/linux/aarch64/libomp.so
    )
endif()

qt_import_qml_plugins(notes)
qt_finalize_executable(notes)
