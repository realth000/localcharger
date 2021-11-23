import QtQuick 2.0
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import TH.QmlHandler 1.0
import "widget"

Item {
    id: mainWorkPageItem
    property int senderState: QmlHandler.SenderDisconnected
    property int recverState: QmlHandler.RecverDisconnected

    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }
    GroupBoxEx {
        id: socketCtlGroupBoxEx
        labelText: "网络"
        labelHeight: 40
        height: startSenderButtonEx.height + startRecverButtonEx.height + titleHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        RowLayout {
            id: socketCtlRowLayout
            height: 40
            spacing: 10
            anchors.top: socketCtlGroupBoxEx.labelRect.bottom
            anchors.topMargin: 10
            anchors.left: socketCtlGroupBoxEx.separator.left
            anchors.right: socketCtlGroupBoxEx.separator.right

            ButtonEx {
                id: startSenderButtonEx
                Layout.preferredWidth: parent.width/2
                Layout.preferredHeight: parent.height
                bgColor: "transparent"
                checkable: false
                texts: "发送"
                textsUncheckedColor: "#f0ffff"
                textsBold: true
                iconUnchecked: getSenderStateIcon()
                iconPos: ButtonEx.IconPos.IconLeft
                iconWidth: 30
                iconHeight: 30
                onClicked: {
                    mainQmlHandler.startSender()
                }
            }
            ButtonEx {
                id: startRecverButtonEx
                Layout.preferredWidth: parent.width/2
                Layout.preferredHeight: parent.height
                bgColor: "transparent"
                checkable: false
                texts: "接收"
                textsUncheckedColor: "#f0ffff"
                textsBold: true
                iconUnchecked: getRecverStateIcon()
                iconPos: ButtonEx.IconPos.IconLeft
                iconWidth: 30
                iconHeight: 30
                onClicked: {
                    mainQmlHandler.startRecver()
                }
            }
        }
    }

    function getSenderStateIcon(){
        switch (senderState) {
        case QmlHandler.SenderDisconnected:
           return "qrc:/pic/disconnected.png"
        case QmlHandler.SenderListening:
           return "qrc:/pic/connecting.png"
        case QmlHandler.SenderConnected:
           return "qrc:/pic/connected.png"
        default:
           return "qrc:/pic/disconnected.png"
        }
    }

    function getRecverStateIcon() {
        switch (recverState) {
        case QmlHandler.RecverDisconnected:
           return "qrc:/pic/disconnected.png"
        case QmlHandler.RecverConnecting:
           return "qrc:/pic/connecting.png"
        case QmlHandler.RecverConnected:
           return "qrc:/pic/connected.png"
        default:
           return "qrc:/pic/disconnected.png"
        }
    }

    function updateSenderState(state) {
        senderState = state
    }

    function updateRecverState(state) {
        recverState = state
    }
}
