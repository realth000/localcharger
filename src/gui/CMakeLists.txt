if(COMPILE_GUI AND NOT COMPILE_QML)
    target_sources(LocalCharger PRIVATE mainui.cpp mainui.ui)
endif()
