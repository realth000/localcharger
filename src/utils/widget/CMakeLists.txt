if(COMPILE_GUI AND NOT COMPILE_QML)
    include_directories(${CMAKE_SOURCE_DIR}/src/include)
    target_sources(LocalCharger PRIVATE messageboxexx.cpp messageboxexx.ui texteditex.cpp titlebar.cpp)
endif()
