target_sources(LocalCharger PRIVATE config.qrc pic.qrc stylesheet.qrc translation.qrc)
if(WIN32)
    target_sources(LocalCharger PRIVATE localcharger.rc)
endif()

if(UNIX)
    target_sources(LocalChargerCli PRIVATE config.qrc)
    target_sources(localcharger-daemon PRIVATE config.qrc)
endif()
