# Localcharger
* Transport files and messages through sockets in LAN.
* Based on Qt5.15.2.
* Available Windows, Linux and Android.

## Build System
* CMake for Windows and Linux.
* QMake mainly for Android.

## v1.7.4
### Known issues
* Transmission is not sync on the two sides in some condition.
* Transmission actually not finished when progress finish signal arrived.
* Lack of processing for files with the same name.
* Require processing when transmission interrupted.

### Todo list
- [ ] Progress bar for total progress sending/receiving directory.
- [ ] Pause, resume and stop transmission.
- [x] Single application.
