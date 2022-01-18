target_sources(LocalChargerCli PRIVATE maincli.cpp clicontroller.h clicontroller.cpp)

target_link_libraries(LocalChargerCli  Qt5::Core Qt5::Network Qt5::WebSockets)
