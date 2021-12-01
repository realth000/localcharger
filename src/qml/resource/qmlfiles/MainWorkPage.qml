import QtQuick 2.15
import QtQml.Models 2.15
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

        contentWidth: parent.width
        contentHeight: socketCtlGroupBoxEx.height + recvedGroupBoxEx.height + sendedGroupBoxEx.height + toSendGroupBoxEx.height + 100

        GroupBoxEx {
            id: socketCtlGroupBoxEx
            labelText: "连接端"
            labelHeight: 40
            iconPath: "qrc:/pic/link2.png"
            height: connectionRowLayout.height + titleHeight + 10 + 10
            anchors.top: parent.top

            RowLayout {
                id: connectionRowLayout
                height: 300
                anchors.top: socketCtlGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: socketCtlGroupBoxEx.separator.left
                anchors.right: socketCtlGroupBoxEx.separator.right

                Rectangle {
                    id: clientViewRectangle
                    Layout.preferredWidth: mainWorkScrollView.width/3
                    Layout.fillHeight: true
                    color: "transparent"
                    border.color: "#f0ffff"
                    ListView {
                        id: clientView
                        anchors.fill: parent
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        clip: true
                        Component {
                            id: clientDelegate
                            Item {
                                id: clientItem
                                width: clientView.width
                                height: 70 + clientItemColumn.topPadding + clientItemColumn.bottomPadding
                                Column {
                                    id: clientItemColumn
                                    leftPadding: 5
                                    rightPadding: 5
                                    topPadding: 5
                                    bottomPadding: 5
                                    Text { color: "#f0ffff"; text: '<b>Name:</b> ' + name }
                                    Text { color: "#f0ffff"; text: '<b>ID:</b> ' + id }
                                    Text { color: "#f0ffff"; text: '<b>IP:</b> ' + ip }
                                    Text { color: "#f0ffff"; text: '<b>Port:</b> ' + port }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: clientView.currentIndex = index
                                }
                            }

                        }
                        model: clientMode
                        delegate: clientDelegate
                        focus: true
                        highlight: RectangleEx {
                            commonBorderColor: false
                            color: "#393939"
                            leftBorderColor: "#f0ffff"
                            rightBorderColor: "#f0ffff"
                            radius: 10
                        }
                    }
                }

                ColumnLayout {
                    id: netStatusColumnLayout
                    Layout.fillWidth: true
                    Layout.preferredHeight: socketSendCtlRowLayout.height + socketRecvCtlRowLayout.height + 60
                    RowLayout {
                        id: socketSendCtlRowLayout
                        height: 40
                        spacing: 0

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
                            leftMargin:(parent.width - spacing)/10
                            enablePressWave: false
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
                            leftMargin:(parent.width - spacing)/10
                            enablePressWave: false
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
            }
        }

        GroupBoxEx {
            id: recvedGroupBoxEx
            labelText: "已接收"
            labelHeight: 40
            iconPath: "qrc:/pic/received2.png"
            height: recvedTextArea.height + titleHeight + 10 + 10
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
                selectByMouse: true
            }
        }

        GroupBoxEx {
            id: sendedGroupBoxEx
            labelText: "已发送"
            labelHeight: 40
            iconPath: "qrc:/pic/sended2.png"
            height: sendedTextArea.height + titleHeight + 10 + 10
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
                selectByMouse: true
            }
        }

        GroupBoxEx {
            id: toSendGroupBoxEx
            labelText: "发文字"
            labelHeight: 40
            iconPath: "qrc:/pic/sendtext2.png"
            height: toSendTextArea.height + titleHeight + 10 + 10
            anchors.top: sendedGroupBoxEx.bottom
            anchors.topMargin: 20
            TextAreaEx {
                id: toSendTextArea
                height: 200
                anchors.top: toSendGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: toSendGroupBoxEx.separator.left
                anchors.right: toSendGroupBoxEx.separator.right
                selectByMouse: true
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
            onClicked: {
                sendFileFileDialogEx.open()
            }
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

    FileDialogEx {
        id: sendFileFileDialogEx
        fontSize: 12
        onChangeSelectedDir: {
            var fileSavePath = selectedPath
            console.log("select file", fileSavePath)
            mainQmlHandler.sendFile(fileSavePath)
        }
    }


    ListModel {
        id: clientMode
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

    function addClient(ip, port, readableName, id) {
        clientMode.append({name: readableName, id: id, ip: ip, port: port })
    }
}
