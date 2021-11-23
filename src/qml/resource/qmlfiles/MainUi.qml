import QtQuick 2.0
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import TH.QmlHandler 1.0
import "widget"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 360
    height: 640
    background: Rectangle {
        color: "#333333"
    }
    ColumnLayout {
        id: mainColumnLayout
        width: mainWindow.width
        height: mainWindow.height
        StackLayout {
            id: mainStackLayout
            currentIndex: mainTabBar.currentIndex
            MainConfigPage {
                id: configPage

            }
            MainWorkPage {

            }
        }

        TabBar {
            id: mainTabBar
            width: mainColumnLayout.width
            height: 70
            spacing: 0
            TabButtonEx {
                id: mainConigPageTabButtonEx
                width: mainTabBar.width/2
                height: mainTabBar.height
                texts: "设定"
                iconChecked: "qrc:/pic/qmlsettings.png"
                iconUnchecked: "qrc:/pic/qmlsettings2.png"
            }
            TabButtonEx {
                id: mainWorkPAgeTabButtonEx
                width: mainTabBar.width/2
                height: mainTabBar.height
                texts: "传送"
                iconChecked: "qrc:/pic/qmlsend.png"
                iconUnchecked: "qrc:/pic/qmlsend2.png"
            }

        }
    }



    QmlHandler {
        id: mainQmlHandler
        onQmlMessageInfo: {
            console.log(msg)
        }
        onQmlUpdateLocalUrlLists: {
            configPage.loadIpUrls(ipStringList);
        }
    }
    Component.onCompleted: {
        mainQmlHandler.initHandler()
    }
}
