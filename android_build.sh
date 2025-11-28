docker build . -t notes-builder
# TODO: figure out owner stuff here so we don't end up with someone else owning build-android
docker run -it --rm -v ${PWD}:/home/user/src:rw notes-builder sh -c "qt-cmake src -B build \
        -DQT_ANDROID_SIGN_APK=ON && \
    cmake --build build && \
    sudo rm -rf src/build-android &&
    sudo cp -r build/ src/build-android"
