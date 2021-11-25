import QtQuick 2.0
import QtQml 2.10
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
            MainWorkPage {
                id: workPage
            }

            MainConfigPage {
                id: configPage

            }
        }

        TabBar {
            id: mainTabBar
            width: mainColumnLayout.width
            height: 70
            spacing: 0
            TabButtonEx {
                id: mainWorkPAgeTabButtonEx
                width: mainTabBar.width/2
                height: mainTabBar.height
                texts: "传送"
                iconChecked: "qrc:/pic/qmlsend.png"
                iconUnchecked: "qrc:/pic/qmlsend2.png"
            }
            TabButtonEx {
                id: mainConigPageTabButtonEx
                width: mainTabBar.width/2
                height: mainTabBar.height
                texts: "设定"
                iconChecked: "qrc:/pic/qmlsettings.png"
                iconUnchecked: "qrc:/pic/qmlsettings2.png"
            }
        }
    }



    QmlHandler {
        id: mainQmlHandler
        // debug
        onQmlMessageInfo: {
            console.log(msg)
        }

        // settings and state
        onQmlUpdateLocalUrlLists: {
            configPage.loadIpUrls(ipStringList)
        }
        onQmlUpdateSocketConfig: {
            configPage.updateSocketConfig(senderIp, senderPort, recverPort)
        }
        onQmlUpdateSenderState: {
            workPage.updateSenderState(state)
            console.log(typeof(state), state, state == QmlHandler.SenderDisconnected)
        }
        onQmlUpdateRecverState: {
            workPage.updateRecverState(state)
            console.log("new recver state =", state)
        }

        // files/messages info display
        onQmlAppendSendedMessage: {
            workPage.appendSendedMessage(msg)
        }
        onQmlAppendRecvedMessage: {
            workPage.appendRecvedMessage(msg)
        }
        onQmlClearToSendMsg: {
            workPage.appendSendedMessage(workPage.getToSendMessage())
            workPage.clearToSendMessage()
        }

        onQmlUpdateFileSavePath: {
           configPage.setFileSavePath(path)
        }
    }

    Component.onCompleted: {
        mainQmlHandler.initHandler()
    }
}
