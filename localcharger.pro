QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 no_batch

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src/include

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/core/main.cpp \
    src/core/webrecver.cpp \
    src/gui/mainui.cpp \
    src/core/websender.cpp \
    src/util/iconinstaller.cpp \
    src/util/qssinstaller.cpp \
    src/util/style/proxystyle.cpp

HEADERS += \
    src/core/webrecver.h \
    src/gui/mainui.h \
    src/core/websender.h \
    src/include/defines.h \
    src/include/iconinstaller.h \
    src/include/proxystyle.h \
    src/include/qssinstaller.h

FORMS += \
    src/gui/mainui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/resource/config.qrc \
    src/resource/stylesheet.qrc
