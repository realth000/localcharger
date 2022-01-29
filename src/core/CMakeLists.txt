if(COMPILE_GUI)
    target_sources(LocalCharger PRIVATE main.cpp jsonparser.cpp threadworker.cpp webidentifier.cpp webrecver.cpp websender.cpp)
endif()

if(UNIX AND COMPILE_DAEMON)
    target_sources(localcharger-daemon PRIVATE jsonparser.cpp threadworker.cpp webidentifier.cpp webrecver.cpp websender.cpp)
endif()
