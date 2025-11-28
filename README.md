# notes
A qt6/kirigami notes app with webdav sync, primarily based on my qualms with other FOSS note apps.
In the end, I want to make a markdown note editor that runs on android and Linux (other platforms probably work, that's just what I use).
I'll be adding webdav support because google makes file syncing on android difficult.
There's a handful of other ideas I have, but I'm aiming to replace my current notes setup first.

## Building
Make sure you have qt6 and kirigami installed

```
cmake -B build
cmake --build build
```

## Android
The android build is now handled by a docker image with qt, ecm, and kirigami installed.
Building should just need:

```
./android_build.sh
```

And then everything will show up inside build-android/
