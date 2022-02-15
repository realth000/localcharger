QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 no_batch

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src src/include

VERSION = 1.6.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = "LocalCharger"
RC_ICONS = "src/resource/pic/LocalCharger.ico"

#DEFINES += COMPILE_VID
DEFINES += COMPILE_QML

win32 {
    CONFIG(debug,debug|release){
        INCLUDEPATH += C:\VisualLeakDetector\include
        DEPENDPATH += C:\VisualLeakDetector\include
        if(contains(DEFINES, COMPILE_VID)) {
            LIBS += -LC:/VisualLeakDetector/lib/Win64 -lvld
            DEFINES += ENABLE_VID
        }
    }
}

android: {
    QT += androidextras
    greaterThan(QT_MAJOR_VERSION, 4): QT -= widgets
}

if(contains(DEFINES, COMPILE_QML)) {
    # QML config
    QT += quick qml
    include( src/qml/qml.pri )
}
else{
    SOURCES += \
        src/gui/mainui.cpp \
        src/utils/iconinstaller.cpp \
        src/utils/qssinstaller.cpp \
        src/utils/style/proxystyle.cpp \
        src/utils/widget/messageboxexx.cpp \
        src/utils/widget/texteditex.cpp \
        src/utils/widget/titlebar.cpp

    HEADERS += \
        src/gui/mainui.h \
        src/include/iconinstaller.h \
        src/include/qssinstaller.h \
        src/include/proxystyle.h \
        src/include/messageboxexx.h \
        src/utils/widget/texteditex.h \
        src/include/titlebar.h

    RESOURCES += \
        src/resource/stylesheet.qrc

    FORMS += \
        src/gui/mainui.ui \
        src/utils/widget/messageboxexx.ui
}
SOURCES += \
    src/core/jsonparser.cpp \
    src/core/main.cpp \
    src/core/threadworker.cpp \
    src/core/webidentifier.cpp \
    src/core/webrecver.cpp \
    src/core/websender.cpp \
    src/utils/filehelper.cpp \
    src/utils/networkinfohelper.cpp \
    src/utils/randomgenerator.cpp

HEADERS += \
    src/core/jsonparser.h \
    src/core/threadworker.h \
    src/core/webidentifier.h \
    src/core/webrecver.h \
    src/core/websender.h \
    src/include/defines.h \
    src/utils/filehelper.h \
    src/utils/networkinfohelper.h \
    src/utils/randomgenerator.h \

RESOURCES += \
    src/resource/pic.qrc \
    src/resource/config.qrc \
    src/resource/translation.qrc

isEmpty(QMAKE_LRELEASE) {
    win32: QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else: QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    unix {
        equals(QT_MAJOR_VERSION, 5) {
            !exists($$QMAKE_LRELEASE): QMAKE_LRELEASE = lrelease-qt5
        }
    }
    else {
        !exists($$QMAKE_LRELEASE): QMAKE_LRELEASE = lrelease
    }
}

lrelease.input = TS_SOURCES
lrelease.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
lrelease.commands = @echo "lrelease ${QMAKE_FILE_NAME}" && $$QMAKE_LRELEASE -silent ${QMAKE_FILE_NAME} -qm ${QMAKE_FILE_OUT}
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

TRANSLATIONS += $$files($$PWD/src/resource/translation/localcharger_*.ts)
TS_SOURCES += $$TRANSLATIONS

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32-msvc* {
  QMAKE_CXXFLAGS += /utf-8
}
android: include(C:/Programming/Android/SDK/android_openssl/openssl.pri)

DISTFILES += \
    src/android/AndroidManifest.xml \
    src/android/build.gradle \
    src/android/gradle.properties \
    src/android/gradle/wrapper/gradle-wrapper.jar \
    src/android/gradle/wrapper/gradle-wrapper.properties \
    src/android/gradlew \
    src/android/gradlew.bat \
    src/android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/src/android
