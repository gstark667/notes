# notes
A qt6/kirigami notes app with webdav sync, primarily based on my qualms with other FOSS note apps.
In the end, I want to make a markdown note editor that runs on android and Linux (other platforms probably work, that's just what I use).
I'll also be adding webdav support because google makes file syncing on android difficult.
There's a handful of other ideas I have, but I'm aiming to replace my current notes setup first.

## Building
Make sure you have qt6 and kirigami installed


```
cmake -B build
cmake --build build
```

## Android
I'm currently just checking out kirigami in the root of this repo (I'll probably add it as a submodule eventually).
Android Sdk/Ndk are in ~/Android. I'm using 29.0.13599879 at the moment
My android build for qt6 is located in ../qt-android-install (from the repo root)

With all that setup, building should just need

```
./android_build.sh
cmake --build build-android
```
