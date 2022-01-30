if(COMPILE_GUI)
    target_sources(LocalCharger PRIVATE config.qrc pic.qrc stylesheet.qrc translation.qrc)
    if(WIN32)
        target_sources(LocalCharger PRIVATE localcharger.rc)
    endif()
endif()

if(UNIX AND COMPILE_CLI)
    target_sources(LocalChargerCli PRIVATE config.qrc)
endif()

if(UNIX AND COMPILE_DAEMON)
    target_sources(localcharger-daemon PRIVATE config.qrc)
endif()
