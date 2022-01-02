target_sources(LocalCharger PRIVATE config.qrc pic.qrc stylesheet.qrc translation.qrc)
if(WIN32)
    target_sources(LocalCharger PRIVATE localcharger.rc)
endif()
