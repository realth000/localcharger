# Localcharger
* Transport files and messages through sockets in LAN.
* Based on Qt5.15.2.
* Support Windows, Linux and Android.

## Build System
* CMake for Windows and Linux.
* QMake mainly for Android.

## v1.6.0
### Known issues
* Collapse in receiving directory on Android.
* Access directory transmission although sockets not connected.
* Error occurs if start auto-connection from Android to Linux(e.g. Manjaro).
* UI hangs when sending file and directory, especially in Qml version.
### Todo lists
* Progress bar for total progress sending/receiving directory.
* Pause, resume and stop transmission.
* Single application.
