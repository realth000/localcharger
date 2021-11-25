import QtQuick 2.0
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import TH.QmlHandler 1.0
import "widget"

Item {
    id: mainWorkPageItem
    property int senderState: QmlHandler.SenderDisconnected
    property int recverState: QmlHandler.RecverDisconnected
    ScrollView {
        id: mainWorkScrollView
        width: parent.width
        height: parent.height - socketMsgCtlRowLayout.height
        clip: true

        background: Rectangle{
            color: "transparent"
            anchors.fill: parent
        }

        contentWidth:parent.width
        contentHeight: socketCtlGroupBoxEx.height + recvedGroupBoxEx.height + sendedGroupBoxEx.height + toSendGroupBoxEx.height + 60

        GroupBoxEx {
            id: socketCtlGroupBoxEx
            labelText: "连接端"
            labelHeight: 40
            iconPath: "qrc:/pic/link2.png"
            height: socketSendCtlRowLayout.height + socketRecvCtlRowLayout.height + titleHeight
            anchors.top: parent.top
            RowLayout {
                id: socketSendCtlRowLayout
                height: 40
                spacing: 0
                anchors.top: socketCtlGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: socketCtlGroupBoxEx.separator.left
                anchors.right: socketCtlGroupBoxEx.separator.right

                ButtonEx {
                    id:  senderStateButtonEx
                    Layout.preferredWidth: (parent.width - spacing)/2
                    Layout.preferredHeight: parent.height
                    bgColor: "transparent"
                    checkable: false
                    texts: getSenderStateString()
                    textsUncheckedColor: "#336666"
                    iconUnchecked: getSenderStateIcon()
                    iconPos: ButtonEx.IconPos.IconLeft
                    posToLeft: true
                    leftMargin:(parent.width - spacing)/6
                }

                ButtonEx {
                    id: startSenderButtonEx
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height
                    bgColor: "transparent"
                    checkable: false
                    texts: "启动发送端"
                    textsUncheckedColor: "#f0ffff"
                    textsBold: true
                    iconUnchecked: "qrc:/pic/start_connect.png"
                    iconPos: ButtonEx.IconPos.IconLeft
                    iconWidth: 30
                    iconHeight: 30
                    onClicked: {
                        mainQmlHandler.startSender()
                    }
                }

            }
            RowLayout {
                id: socketRecvCtlRowLayout
                height: 40
                spacing: 0
                anchors.top: socketSendCtlRowLayout.bottom
                anchors.topMargin: 10
                anchors.left: socketCtlGroupBoxEx.separator.left
                anchors.right: socketCtlGroupBoxEx.separator.right

                ButtonEx {
                    id:  recverStateButtonEx
                    Layout.preferredWidth: (parent.width - spacing)/2
                    Layout.preferredHeight: parent.height
                    bgColor: "transparent"
                    checkable: false
                    texts: getRecverStateString()
                    textsUncheckedColor: "#336666"
                    iconUnchecked: getRecverStateIcon()
                    iconPos: ButtonEx.IconPos.IconLeft
                    posToLeft: true
                    leftMargin:(parent.width - spacing)/6
                }

                ButtonEx {
                    id: startRecverButtonEx
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height
                    bgColor: "transparent"
                    checkable: false
                    texts: "启动接收端"
                    textsUncheckedColor: "#f0ffff"
                    textsBold: true
                    iconUnchecked: "qrc:/pic/start_connect.png"
                    iconPos: ButtonEx.IconPos.IconLeft
                    iconWidth: 30
                    iconHeight: 30
                    onClicked: {
                        mainQmlHandler.startRecver()
                    }
                }
            }

        }

        GroupBoxEx {
            id: recvedGroupBoxEx
            labelText: "已接收"
            labelHeight: 40
            iconPath: "qrc:/pic/received2.png"
            height: recvedTextArea.height + titleHeight
            anchors.top: socketCtlGroupBoxEx.bottom
            anchors.topMargin: 20
            TextAreaEx {
                id: recvedTextArea
                height: 200
                readOnly: true
                anchors.top: recvedGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: recvedGroupBoxEx.separator.left
                anchors.right: recvedGroupBoxEx.separator.right
            }
        }

        GroupBoxEx {
            id: sendedGroupBoxEx
            labelText: "已发送"
            labelHeight: 40
            iconPath: "qrc:/pic/sended2.png"
            height: sendedTextArea.height + titleHeight
            anchors.top: recvedGroupBoxEx.bottom
            anchors.topMargin: 20
            TextAreaEx {
                id: sendedTextArea
                height: 200
                readOnly: true
                anchors.top: sendedGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: sendedGroupBoxEx.separator.left
                anchors.right: sendedGroupBoxEx.separator.right
            }
        }

        GroupBoxEx {
            id: toSendGroupBoxEx
            labelText: "发文字"
            labelHeight: 40
            iconPath: "qrc:/pic/sendtext2.png"
            height: toSendTextArea.height + titleHeight
            anchors.top: sendedGroupBoxEx.bottom
            anchors.topMargin: 20
            TextAreaEx {
                id: toSendTextArea
                height: 200
                anchors.top: toSendGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: toSendGroupBoxEx.separator.left
                anchors.right: toSendGroupBoxEx.separator.right
            }
        }
    }
    RowLayout {
        id: socketMsgCtlRowLayout
        width: parent.width
        height: 60
        anchors.bottom: parent.bottom
        ButtonEx {
            id: sendFileButtonEx
            Layout.preferredWidth: parent.width/2
            Layout.preferredHeight: parent.height
            bgColor: "transparent"
            checkable: false
            texts: "发文件"
            textsUncheckedColor: "#f0ffff"
            textsBold: true
            iconUnchecked: "qrc:/pic/send_file.png"
            iconPos: ButtonEx.IconPos.IconLeft
            iconWidth: 30
            iconHeight: 30
        }
        ButtonEx {
            id: sendMessageButtonEx
            Layout.preferredWidth: parent.width/2
            Layout.preferredHeight: parent.height
            bgColor: "transparent"
            checkable: false
            texts: "发信息"
            textsUncheckedColor: "#f0ffff"
            textsBold: true
            iconUnchecked: "qrc:/pic/send.png"
            iconPos: ButtonEx.IconPos.IconLeft
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                mainQmlHandler.sendMessage(toSendTextArea.text)
            }

        }
    }


    function getSenderStateString(){
        switch (senderState) {
        case QmlHandler.SenderDisconnected:
           return "closed"
        case QmlHandler.SenderListening:
           return "listening"
        case QmlHandler.SenderConnected:
           return "connected"
        default:
           return "closed"
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

    function getRecverStateString() {
        switch (recverState) {
        case QmlHandler.RecverDisconnected:
           return "disconnected"
        case QmlHandler.RecverConnecting:
           return "connecting"
        case QmlHandler.RecverConnected:
           return "connected"
        default:
           return "disconnected"
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

    function getToSendMessage() {
        return toSendTextArea.text
    }

    function clearToSendMessage() {
        toSendTextArea.clear()
    }

    function appendSendedMessage(msg) {
        sendedTextArea.append(msg)
    }

    function appendRecvedMessage(msg) {
        recvedTextArea.append(msg)
    }
}
