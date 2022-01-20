target_sources(LocalCharger PRIVATE main.cpp jsonparser.cpp threadworker.cpp webidentifier.cpp webrecver.cpp websender.cpp)
if(UNIX)
    target_sources(localcharger-daemon PRIVATE jsonparser.cpp threadworker.cpp webidentifier.cpp webrecver.cpp websender.cpp)
endif()
