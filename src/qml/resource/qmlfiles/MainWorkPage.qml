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
        contentHeight: socketCtlGroupBoxEx.height + recvedGroupBoxEx.height + sendedGroupBoxEx.height + 50

        GroupBoxEx {
            id: socketCtlGroupBoxEx
            labelText: "网络"
            labelHeight: 40
            height: socketCtlRowLayout.height + titleHeight
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

        GroupBoxEx {
            id: recvedGroupBoxEx
            labelText: "已接收"
            labelHeight: 40
            height: recvedTextArea.height + titleHeight
            anchors.top: socketCtlGroupBoxEx.bottom
            anchors.topMargin: 10
            anchors.left: socketCtlGroupBoxEx.left
            anchors.right: socketCtlGroupBoxEx.right
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
            height: sendedTextArea.height + titleHeight
            anchors.top: recvedGroupBoxEx.bottom
            anchors.topMargin: 10
            anchors.left: recvedGroupBoxEx.left
            anchors.right: recvedGroupBoxEx.right
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
            labelText: "待发送"
        }
    }
    RowLayout {
        id: socketMsgCtlRowLayout
        width: parent.width
        height: 40
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
