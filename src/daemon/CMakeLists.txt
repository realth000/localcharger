target_sources(localcharger-daemon PRIVATE maindaemon.cpp daemon.h daemon.cpp)

target_link_libraries(localcharger-daemon  Qt5::Core Qt5::Network Qt5::WebSockets Qt5::DBus qhttpserver)
