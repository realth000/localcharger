import QtQuick 2.0
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import "widget"

Item {
    // settings
    property string remoteUrl: ""
    property int remotePort: 1234
    property int localPort: 1234

    readonly property QtObject root: mainConfigPageItem
    property string fileSavePath

    id: mainConfigPageItem
    // background
    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }

    GroupBoxEx {
        id: localGroupBoxEx
        labelText: "接收"
        labelHeight: 40
        iconPath: "qrc:/pic/received2.png"
        height:localUrlRowLayout.height + localPortRowLayout.height + titleHeight
        anchors.top: parent.top
        RowLayout {
            id: localUrlRowLayout
            height: 40
            spacing: 10
            anchors.top: localGroupBoxEx.labelRect.bottom
            anchors.topMargin: 10
            anchors.left: localGroupBoxEx.separator.left
            anchors.right: localGroupBoxEx.separator.right
            Text {
                id: localUrlText
                Layout.preferredWidth: 100
                Layout.preferredHeight: parent.height
                text: "查看本地IP"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: "#f0ffff"
            }
            ComboBoxEx {
                id: localUrlComboBoxEx
                Layout.preferredHeight: parent.height
                Layout.fillWidth: true
            }
        }

        RowLayout {
            id: localPortRowLayout
            height: localUrlRowLayout.height
            spacing: localUrlRowLayout.spacing
            anchors.top: localUrlRowLayout.bottom
            anchors.left: localUrlRowLayout.left
            anchors.topMargin: 10
            anchors.right: localUrlRowLayout.right
            Text {
                id: localPortText
                Layout.preferredWidth: localUrlText.width
                Layout.preferredHeight: localUrlText.height
                text: "本地端口"
                horizontalAlignment: localUrlText.horizontalAlignment
                verticalAlignment: localUrlText.verticalAlignment
                font.pixelSize: localUrlText.font.pixelSize
                color: localUrlText.color
            }
            TextFieldEx {
                id: locatPortTextFieldEx
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: remoteUrlText.color
                validator: IntValidator {
                    top: 65535
                    bottom: 0
                }
                onTextEdited: {
                    mainQmlHandler.setRecverPort(text)
                    mainQmlHandler.saveConfig()
                }
            }
        }
    }

    GroupBoxEx {
        id: remoteGroupBoxEx
        labelText: "发送"
        iconPath: "qrc:/pic/sended2.png"
        labelHeight: localGroupBoxEx.labelHeight
        height:remoteUrlRowLayout.height + remotePortRowLayout.height + titleHeight + 20
        anchors.top: localGroupBoxEx.bottom
        anchors.topMargin: 20

        RowLayout {
            id: remoteUrlRowLayout
            height: 40
            spacing: 10
            anchors.top: remoteGroupBoxEx.labelRect.bottom
            anchors.topMargin: 10
            anchors.left: remoteGroupBoxEx.separator.left
            anchors.right: remoteGroupBoxEx.separator.right

            Text {
                id: remoteUrlText
                Layout.preferredWidth: 100
                Layout.preferredHeight: parent.height
                text: "远端IP"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: localUrlText.color
            }
            TextFieldEx {
                id: remoteUrlTextFieldEx
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: remoteUrlText.color
                validator: RegExpValidator {
                    regExp: /^((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))|localhost$/
                }
                onTextEdited: {
                    mainQmlHandler.setSenderUrl(text)
                    mainQmlHandler.saveConfig()
                }
            }
        }

        RowLayout {
            id: remotePortRowLayout
            height: remoteUrlRowLayout.height
            spacing: remoteUrlRowLayout.spacing
            anchors.top: remoteUrlRowLayout.bottom
            anchors.topMargin: 20
            anchors.left: remoteUrlRowLayout.left
            anchors.right: remoteUrlRowLayout.right
            Text {
                id: remotePortText
                Layout.preferredWidth: localUrlText.width
                Layout.preferredHeight: localUrlText.height
                text: "远端端口"
                horizontalAlignment: localUrlText.horizontalAlignment
                verticalAlignment: localUrlText.verticalAlignment
                font.pixelSize: localUrlText.font.pixelSize
                color: localUrlText.color
            }
            TextFieldEx {
                id: remotePortTextFieldEx
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: remoteUrlText.color
                onTextEdited: {
                    mainQmlHandler.setSenderPort(text)
                    mainQmlHandler.saveConfig()
                }
                validator: IntValidator {
                    top: 65535
                    bottom: 0
                }
            }
        }
    }
    GroupBoxEx {
        id: selevtSaveDirGroupBoxEx
        labelText: "保存位置"
        labelHeight: localGroupBoxEx.labelHeight
        anchors.top: remoteGroupBoxEx.bottom
        anchors.topMargin: 20
        anchors.bottom: parent.bottom
        iconPath: "qrc:/pic/openfolder2.png"
        TextFieldEx {
            id: selectSaveDirButtonEx
            height: 40
            clip: true
            anchors.top: selevtSaveDirGroupBoxEx.labelRect.bottom
            anchors.topMargin: 10
            anchors.left: selevtSaveDirGroupBoxEx.separator.left
            anchors.right: selevtSaveDirGroupBoxEx.separator.right
            bgColor: "transparent"
            readOnly: true
            text: fileSavePath
            onReleased: {
                setSavePathFileDialogEx.open()
            }
        }
    }

    FileDialogEx {
        id: setSavePathFileDialogEx
        fontSize: 12
        workMode: FileDialogEx.WorkMode.SelectDirOnly
        onChangeSelectedDir: {
            fileSavePath = selectedPath
            console.log("select dir", fileSavePath)
            mainQmlHandler.setFileSavePath(fileSavePath)
            mainQmlHandler.saveConfig()
        }
    }

    function loadIpUrls(ipStringList) {
        localUrlComboBoxEx.resetModel(ipStringList)
    }

    function updateSocketConfig(senderIp, senderPort, recverPort) {
        remoteUrl = senderIp
        remoteUrlTextFieldEx.text = remoteUrl
        remotePort = senderPort
        remotePortTextFieldEx.text = remotePort
        localPort = recverPort
        locatPortTextFieldEx.text = localPort
    }

    function setFileSavePath(path) {
        fileSavePath = path
    }

}
