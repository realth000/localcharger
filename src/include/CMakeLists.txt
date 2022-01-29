if(COMPILE_GUI)
    target_sources(LocalCharger PUBLIC defines.h)
    if(NOT COMPILE_QML)
        target_sources(LocalCharger PRIVATE iconinstaller.h qssinstaller.h proxystyle.h messageboxexx.h titlebar.h)
    endif()
endif()

if(UNIX AND COMPILE_CLI)
    target_sources(LocalChargerCli PUBLIC defines.h)
endif()

if(UNIX AND COMPILE_DAEMON)
    target_sources(localcharger-daemon PUBLIC defines.h)
endif()
