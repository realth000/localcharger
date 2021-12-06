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
            height: clientNameColumnLayout.height + titleHeight + 10 + 10
            anchors.top: parent.top
            ColumnLayout {
                id: clientNameColumnLayout
                width: parent.width
                height: 340
                anchors.top: socketCtlGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: socketCtlGroupBoxEx.separator.left
                anchors.right: socketCtlGroupBoxEx.separator.right
                Rectangle {
                    id: clientNameRectangle
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "transparent"
                    RowLayout {
                        width: parent.width
                        height: parent.height
                        spacing: 10
                        Text {
                            id: clientNameText
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: parent.height
                            text: "本地名称"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 16
                            color: "#f0ffff"
                        }
                        TextFieldEx {
                            id: clientNameTextFieldEx
                            Layout.fillWidth: true
                            Layout.preferredHeight: parent.height
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 16
                            color: clientNameText.color
                            onTextEdited: {
                                mainQmlHandler.setClientReadableName(text)
                                mainQmlHandler.saveConfig()
                            }
                        }
                    }
                }
                RowLayout {
                    id: connectionRowLayout
                    height: 300
                    anchors.topMargin: 10
                    Rectangle {
                        id: clientViewRectangle
                        Layout.preferredWidth: mainWorkScrollView.width/2
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
                                    property string clientName: name
                                    property string clientId: id
                                    property string clientIp: ip
                                    property string clientPort: port
                                    Column {
                                        id: clientItemColumn
                                        width: parent.width
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
                            highlightMoveDuration: 200
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
                            id: clientsLayOut
                            height: 60
                            spacing: 0
                            ButtonEx {
                                id: broadcastButtonEx
                                Layout.preferredWidth: (parent.width - spacing)/2
                                Layout.preferredHeight: 60
                                bgColor: "transparent"
                                checkable: false
                                texts: "广播"
                                textsUncheckedColor: "#f0ffff"
                                iconUnchecked: "qrc:/pic/broadcast.png"
                                iconPos: ButtonEx.IconPos.IconLeft
                                onClicked: {
                                    mainQmlHandler.boardcastIdentityMessage()
                                }
                            }
                            ButtonEx {
                                id: connectClientButtonEx
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                bgColor: "transparent"
                                checkable: false
                                texts: "连接"
                                textsUncheckedColor: "#f0ffff"
                                iconUnchecked: "qrc:/pic/start_connect.png"
                                iconPos: ButtonEx.IconPos.IconLeft
                                onClicked: {
                                    var sName = clientView.currentItem.clientName
                                    var sId = clientView.currentItem.clientId
                                    var sIp = clientView.currentItem.clientIp
                                    var sPort = clientView.currentItem.clientPort
                                    console.log("start connect to", clientView.currentItem.clientName, clientView.currentItem.clientId, clientView.currentItem.clientIp, clientView.currentItem.clientPort)
                                    if(sIp === "" || sPort === "") {
                                        console.log("empty client")
                                        return
                                    }
                                    mainQmlHandler.setSenderUrl(sIp)
                                    mainQmlHandler.setSenderPort(sPort)
                                    configPage.updateRemoteConfig(sIp, sPort)
                                    mainQmlHandler.connectSelectedClient(sName, sId, sIp, sPort)
                                }
                            }
                        }

                        ColumnLayout {
                            id: socketSendCtlRowLayout
                            width: parent.width
                            spacing: 0

                            ButtonEx {
                                id:  senderStateButtonEx
                                Layout.fillWidth: true
                                Layout.preferredHeight: (parent.height - spacing)/2
                                bgColor: "transparent"
                                checkable: false
                                texts: getSenderStateString()
                                textsUncheckedColor: "#336666"
                                iconUnchecked: getSenderStateIcon()
                                iconPos: ButtonEx.IconPos.IconLeft
                                posToLeft: true
                                leftMargin:(parent.width - spacing)/5
                                enablePressWave: false
                            }
                            ButtonEx {
                                id: startSenderButtonEx
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                bgColor: "transparent"
                                checkable: false
                                texts: "启动发送端"
                                textsUncheckedColor: "#f0ffff"
                                textsBold: true
                                iconUnchecked: "qrc:/pic/start_connect.png"
                                iconPos: ButtonEx.IconPos.IconLeft
                                iconWidth: 30
                                iconHeight: 30
                                posToLeft: true
                                leftMargin:(parent.width - spacing)/5
                                onClicked: {
                                    mainQmlHandler.startSender()
                                }
                            }
                        }
                        ColumnLayout {
                            id: socketRecvCtlRowLayout
                            width: parent.width
                            spacing: 0

                            ButtonEx {
                                id:  recverStateButtonEx
                                Layout.fillWidth: true
                                Layout.preferredHeight: (parent.height - spacing)/2
                                bgColor: "transparent"
                                checkable: false
                                texts: getRecverStateString()
                                textsUncheckedColor: "#336666"
                                iconUnchecked: getRecverStateIcon()
                                iconPos: ButtonEx.IconPos.IconLeft
                                posToLeft: true
                                leftMargin:(parent.width - spacing)/5
                                enablePressWave: false
                            }
                            ButtonEx {
                                id: startRecverButtonEx
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                bgColor: "transparent"
                                checkable: false
                                texts: "启动接收端"
                                textsUncheckedColor: "#f0ffff"
                                textsBold: true
                                iconUnchecked: "qrc:/pic/start_connect.png"
                                iconPos: ButtonEx.IconPos.IconLeft
                                iconWidth: 30
                                iconHeight: 30
                                posToLeft: true
                                leftMargin:(parent.width - spacing)/5
                                onClicked: {
                                    mainQmlHandler.startRecver()
                                }
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
            height: recvedTextRectangle.height + titleHeight + 10 + 10
            anchors.top: socketCtlGroupBoxEx.bottom
            anchors.topMargin: 20
            Rectangle {
                id: recvedTextRectangle
                height: 200
                anchors.top: recvedGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: recvedGroupBoxEx.separator.left
                anchors.right: recvedGroupBoxEx.separator.right
                color: "transparent"
                border.color: "#336666"
                Flickable {
                    id: recvedTextFlickable
                    anchors.fill: parent
                    boundsBehavior: Flickable.StopAtBounds
                    contentWidth: Math.max(recvedTextRectangle.width, maxWidth)
                    contentHeight: parent.height
                    clip: true
                    property int maxWidth: 0
                    ListView {
                        id: recvedTextListView
                        anchors.fill: parent
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        clip: true
                        Component {
                            id: recvedTextDelegate
                            Text {
                                id: recvedTextText
                                height: font.pixelSize*getTextLines
                                width: Math.max(recvedTextRectangle.width, recvedTextFlickable.maxWidth)
                                leftPadding: 5
                                rightPadding: 5
                                topPadding: 10
                                bottomPadding: 10
                                color: "#f0ffff"
                                text: msg
                                font.pixelSize: 14
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: { recvedTextListView.currentIndex = index }
                                    onPressAndHold: { mainQmlHandler.setClipBoardText(msg) }
                                }
                                function getTextLines(text){
                                    var length = text.length
                                    var lineCount = 1
                                    for(let i=0; i<length; i++) {
                                        if(text[i] === "\n") {
                                            lineCount++
                                        }
                                    }
                                }
                                Component.onCompleted: {
                                    if(recvedTextFlickable.maxWidth < recvedTextText.contentWidth + 10) {
                                        recvedTextFlickable.maxWidth = recvedTextText.contentWidth + 10
                                    }
                                }
                            }
                        }
                        ListModel {
                            id: recvedTextModel
                        }
                        model: recvedTextModel
                        delegate: recvedTextDelegate
                        focus: true
                        highlightMoveDuration: 200
                        highlightResizeVelocity: -1
                        highlightFollowsCurrentItem: false
                        highlight: RectangleEx {
                            width: recvedTextRectangle.width
                            height: recvedTextListView.currentItem.height
                            x: recvedTextFlickable.visibleArea.xPosition * recvedTextFlickable.contentWidth
                            y: recvedTextListView.currentItem.y
                            commonBorderColor: false
                            color: "#393939"
                            leftBorderColor: "#336666"
                            rightBorderColor: "#336666"
                            radius: 10
                        }
                    }
                }
            }
        }

        GroupBoxEx {
            id: sendedGroupBoxEx
            labelText: "已发送"
            labelHeight: 40
            iconPath: "qrc:/pic/sended2.png"
            height: sendedTextRectangle.height + titleHeight + 10 + 10
            anchors.top: recvedGroupBoxEx.bottom
            anchors.topMargin: 20
            Rectangle {
                id: sendedTextRectangle
                height: 200
                anchors.top: sendedGroupBoxEx.labelRect.bottom
                anchors.topMargin: 10
                anchors.left: sendedGroupBoxEx.separator.left
                anchors.right: sendedGroupBoxEx.separator.right
                color: "transparent"
                border.color: "#336666"
                Flickable {
                    id: sendedTextFlickable
                    anchors.fill: parent
                    boundsBehavior: Flickable.StopAtBounds
                    contentWidth: Math.max(sendedTextRectangle.width, maxWidth)
                    contentHeight: parent.height
                    clip: true
                    property int maxWidth: 0
                    ListView {
                        id: sendedTextListView
                        anchors.fill: parent
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        clip: true
                        Component {
                            id: sendedTextDelegate
                            Text {
                                id: sendedTextText
                                height: font.pixelSize*getTextLines
                                width: Math.max(sendedTextRectangle.width, sendedTextFlickable.maxWidth)
                                leftPadding: 5
                                rightPadding: 5
                                topPadding: 10
                                bottomPadding: 10
                                color: "#f0ffff"
                                text: msg
                                font.pixelSize: 14
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: { sendedTextListView.currentIndex = index }
                                    onPressAndHold: { mainQmlHandler.setClipBoardText(msg) }
                                }
                                function getTextLines(text){
                                    var length = text.length
                                    var lineCount = 1
                                    for(let i=0; i<length; i++) {
                                        if(text[i] === "\n") {
                                            lineCount++
                                        }
                                    }
                                }
                                Component.onCompleted: {
                                    if(sendedTextFlickable.maxWidth < sendedTextText.contentWidth + 10) {
                                        sendedTextFlickable.maxWidth = sendedTextText.contentWidth + 10
                                    }
                                }
                            }
                        }
                        ListModel {
                            id: sendedTextModel
                        }
                        model: sendedTextModel
                        delegate: sendedTextDelegate
                        focus: true
                        highlightMoveDuration: 200
                        highlightResizeVelocity: -1
                        highlightFollowsCurrentItem: false
                        highlight: RectangleEx {
                            width: sendedTextRectangle.width
                            height: sendedTextListView.currentItem.height
                            x: sendedTextFlickable.visibleArea.xPosition * sendedTextFlickable.contentWidth
                            y: sendedTextListView.currentItem.y
                            commonBorderColor: false
                            color: "#393939"
                            leftBorderColor: "#336666"
                            rightBorderColor: "#336666"
                            radius: 10
                        }
                    }
                }
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

    function updateClientName(name) {
        clientNameTextFieldEx.text = name
    }

    function getToSendMessage() {
        return toSendTextArea.text
    }

    function clearToSendMessage() {
        toSendTextArea.clear()
    }

    function appendSendedMessage(msg) {
        sendedTextModel.append({msg: msg})
    }

    function appendRecvedMessage(msg) {
        recvedTextModel.append({msg: msg})
    }

    function addClient(ip, port, readableName, id) {
        clientMode.append({name: readableName, id: id, ip: ip, port: port })
    }
}
