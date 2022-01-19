target_sources(LocalCharger PUBLIC defines.h)
if(NOT COMPILE_QML)
    target_sources(LocalCharger PRIVATE iconinstaller.h qssinstaller.h proxystyle.h messageboxexx.h titlebar.h)
endif()

target_sources(LocalChargerCli PUBLIC defines.h)
target_sources(localcharger-daemon PUBLIC defines.h)
