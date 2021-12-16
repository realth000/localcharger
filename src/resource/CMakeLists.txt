#set(RESOURCES config.qrc pic.qrc stylesheet.qrc)
#
#foreach(SOURCE IN LISTS RESOURCES)
#    target_sources(LocalCharger ${CMAKE_CURRENT_LIST_DIR}/${SOURCE})
#endforeach(SOURCE IN RESOURCES)
target_sources(LocalCharger PRIVATE config.qrc pic.qrc stylesheet.qrc)
if(WIN32)
    target_sources(LocalCharger PRIVATE localcharger.rc)
endif()
