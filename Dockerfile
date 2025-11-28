# TODO: probably need to include a copy of their build so we can control versions better
FROM a12e/docker-qt:6.8-android

ARG DEBIAN_FRONTEND=noninteractive
ARG ECM_TAG=v6.7.0
ARG KIRIGAMI_TAG=v6.7.0
ARG GRADLE_VERSION=8.10

ENV KDE_INSTALL_PREFIX=/opt/kde \
    CMAKE_PREFIX_PATH=/opt/kde:${CMAKE_PREFIX_PATH} \
    QT_ADDITIONAL_PACKAGES_PREFIX_PATH=/opt/kde:${QT_ADDITIONAL_PACKAGES_PREFIX_PATH} \
    PATH=/opt/kde/bin:/opt/gradle/gradle-${GRADLE_VERSION}/bin:${PATH} \
    ECM_DIR=/opt/kde/lib/cmake/ECM \
    KF6_DIR=/opt/kde/lib/cmake \
    KF6Kirigami_DIR=/opt/kde/lib/cmake/KF6Kirigami \
    KF6KirigamiPlatform_DIR=/opt/kde/lib/cmake/KF6KirigamiPlatform \
    QML_IMPORT_PATH=/opt/kde/lib/qml:${QML_IMPORT_PATH} \
    QML2_IMPORT_PATH=/opt/kde/lib/qml:${QML2_IMPORT_PATH} \
    PKG_CONFIG_PATH=/opt/kde/lib/pkgconfig:/opt/kde/lib64/pkgconfig:${PKG_CONFIG_PATH} \
    GRADLE_HOME=/opt/gradle/gradle-${GRADLE_VERSION}

# TODO: Make this better. Works fine for development, but would never work for release builds.
ARG KEYSTORE_ALIAS=notes
ARG KEYSTORE_PASSWORD=notespass
ARG KEYSTORE_DNAME="CN=notes,O=gstark,L=City,ST=State,C=US"
ENV QT_ANDROID_KEYSTORE_PATH=/opt/android/keystore/notes.keystore \
    QT_ANDROID_KEYSTORE_ALIAS=${KEYSTORE_ALIAS} \
    QT_ANDROID_KEYSTORE_STORE_PASS=${KEYSTORE_PASSWORD} \
    QT_ANDROID_KEYSTORE_KEY_PASS=${KEYSTORE_PASSWORD}
RUN sudo mkdir -p /opt/android/keystore \
    && sudo chown ${USER}:${USER} /opt/android/keystore \
    && sudo keytool -genkeypair \
    -alias ${KEYSTORE_ALIAS} \
    -keyalg RSA -keysize 2048 -validity 10000 \
    -keystore /opt/android/keystore/notes.keystore \
    -storepass ${KEYSTORE_PASSWORD} \
    -keypass ${KEYSTORE_PASSWORD} \
    -dname "${KEYSTORE_DNAME}"

RUN sudo apt-get update && sudo apt-get install -y --no-install-recommends \
    build-essential \
    ninja-build \
    git \
    cmake \
    python3 \
    pkg-config \
    libgl1-mesa-dev \
    libx11-dev \
    wget

RUN git clone --depth 1 --branch ${ECM_TAG} https://invent.kde.org/frameworks/extra-cmake-modules.git
RUN cd extra-cmake-modules \
    && cmake -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=${KDE_INSTALL_PREFIX} \
    && sudo cmake --build build --target install

RUN git clone --depth 1 --branch ${KIRIGAMI_TAG} https://invent.kde.org/frameworks/kirigami.git
RUN cd kirigami \
    && qt-cmake -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DCMAKE_INSTALL_PREFIX=${KDE_INSTALL_PREFIX} \
    -DCMAKE_PREFIX_PATH="${KDE_INSTALL_PREFIX}:${CMAKE_PREFIX_PATH}" \
    -DECM_DIR=/opt/kde/share/ECM/cmake \
    -DCMAKE_CXX_FLAGS="-UQT_NO_CAST_FROM_ASCII ${CMAKE_CXX_FLAGS}" \
    && sudo cmake --build build --target install

RUN sudo rm -rf kirigami extra-cmake-modules

# Downloads gradle so it doesn't have to download every time you build
RUN /opt/qt/6.8.0/android_arm64_v8a/src/3rdparty/gradle/gradlew --version
