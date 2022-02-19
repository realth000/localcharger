# Localcharger
* Transport files and messages through sockets in LAN.
* Based on Qt5.15.2.
* Support Windows, Linux and Android.

## Build System
* CMake for Windows and Linux.
* QMake mainly for Android.

## v1.7.2
### Known issues
* ~~Files and directories NOT saved to disk on Android.~~
* Second connection from daemon to gui will cause gui collapse.
* ~~missing Cursor after send files or directories through daemon.~~

### Todo list
- [ ] Progress bar for total progress sending/receiving directory.
- [ ] Pause, resume and stop transmission.
- [x] Single application.
