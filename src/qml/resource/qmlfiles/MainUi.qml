import QtQuick 2.0
import QtQml 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import TH.QmlHandler 1.0
import "widget"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 540
    height: 960
    background: Rectangle {
        color: "#272727"
    }
    ColumnLayout {
        id: mainColumnLayout
        width: mainWindow.width
        height: mainWindow.height
        StackLayout {
            id: mainStackLayout
            currentIndex: mainTabBar.currentIndex
            width: parent.width
            Layout.alignment: Qt.AlignBottom
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
                iconChecked: "qrc:/pic/qmlsend2.png"
                iconUnchecked: "qrc:/pic/qmlsend.png"
            }
            TabButtonEx {
                id: mainConigPageTabButtonEx
                width: mainTabBar.width/2
                height: mainTabBar.height
                texts: "设置"
                iconChecked: "qrc:/pic/qmlsettings2.png"
                iconUnchecked: "qrc:/pic/qmlsettings.png"
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
        onQmlUpdateClientAutoConnect: {
            configPage.updateClientAutoConnect(isEnabled)
        }

        onQmlUpdateSenderState: {
            workPage.updateSenderState(state)
        }
        onQmlUpdateRecverState: {
            workPage.updateRecverState(state)
        }
        onQmlUpdateClientName: {
            workPage.updateClientName(name)
        }

        onQmlUpdateClientId: {
            workPage.updateClientId(id)
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

        // WebIdentifier
        onQmlAddClient: {
            workPage.addClient(ip, port, readableName, id)
        }
    }

    Component.onCompleted: {
        mainQmlHandler.initHandler()
    }
}
