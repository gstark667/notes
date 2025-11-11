export ANDROID_NDK=~/Android/Sdk/ndk/29.0.13599879/
export ANDROID_NDK_ROOT=~/Android/Sdk/ndk/29.0.13599879/
export ANDROID_SDK_ROOT=~/Android/Sdk/

if [ -z "$QT6_PATH" ];
then export QT6_PATH=$(pwd)/../qt-android-install
fi

# TODO: there's gotta be a better way to do this, but idk what it is right now
cmake -B build-android -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-33 \
    -DANDROID_SDK_ROOT=/home/octalus/Android/Sdk/ \
    -DQt6_DIR=$QT6_PATH/lib/cmake/Qt6 \
    -DQt6Core_DIR=$QT6_PATH/lib/cmake/Qt6Core \
    -DQt6Gui_DIR=$QT6_PATH/lib/cmake/Qt6Gui \
    -DQt6Qml_DIR=$QT6_PATH/lib/cmake/Qt6Qml \
    -DQt6QmlIntegration_DIR=$QT6_PATH/lib/cmake/Qt6QmlIntegration \
    -DQt6Network_DIR=$QT6_PATH/lib/cmake/Qt6Network \
    -DQt6Quick_DIR=$QT6_PATH/lib/cmake/Qt6Quick \
    -DQt6QmlMeta_DIR=$QT6_PATH/lib/cmake/Qt6QmlMeta \
    -DQt6QmlModels_DIR=$QT6_PATH/lib/cmake/Qt6QmlModels \
    -DQt6QmlWorkerScript_DIR=$QT6_PATH/lib/cmake/Qt6QmlWorkerScript \
    -DQt6OpenGL_DIR=$QT6_PATH/lib/cmake/Qt6OpenGL \
    -DQt6Svg_DIR=$QT6_PATH/lib/cmake/Qt6Svg \
    -DQt6QuickControls2_DIR=$QT6_PATH/lib/cmake/Qt6QuickControls2 \
    -DQt6QuickTemplates2_DIR=$QT6_PATH/lib/cmake/Qt6QuickTemplates2 \
    -DQt6Concurrent_DIR=$QT6_PATH/lib/cmake/Qt6Concurrent \
    -DQt6ShaderTools_DIR=$QT6_PATH/lib/cmake/Qt6ShaderTools \
    -DQt6GuiPrivate_DIR=$QT6_PATH/lib/cmake/Qt6GuiPrivate \
    -DQt6CorePrivate_DIR=$QT6_PATH/lib/cmake/Qt6CorePrivate \
    -DQt6QmlAssetDownloader_DIR=$QT6_PATH/lib/cmake/Qt6QmlAssetDownloader \
    -DQt6ExamplesAssetDownloaderPrivate_DIR=$QT6_PATH/lib/cmake/Qt6ExamplesAssetDownloaderPrivate \
    -DKF6Kirigami_DIR=/home/octalus/Programming/notes/build-android/kirigami \
    -DECM_DIR=/usr/share/ECM/cmake
