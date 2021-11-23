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

    id: mainConfigPageItem
    // background
    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }

    GroupBoxEx {
        id: localGroupBoxEx
        labelText: "发送"
        labelHeight: 40
        height:localUrlRowLayout.height + localPortRowLayout.height + titleHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
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
            Text {
                id: locatPortText2
                Layout.preferredHeight: localUrlComboBoxEx.height
                Layout.fillWidth: true
                text: localPort
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: localUrlText.font.pixelSize
                color: localUrlText.color
            }
        }
    }

    GroupBoxEx {
        id: remoteGroupBoxEx
        labelText: "接收"
        labelHeight: localGroupBoxEx.labelHeight
        height:remoteUrlRowLayout.height + remotePortRowLayout.height + titleHeight
        anchors.top: localGroupBoxEx.bottom
        anchors.left: localGroupBoxEx.left
        anchors.right: localGroupBoxEx.right

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
            Text {
                id: remoteUrlText2
                Layout.preferredWidth: 100
                Layout.preferredHeight: parent.height
                text: remoteUrl
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: remoteUrlText.color
            }
        }

        RowLayout {
            id: remotePortRowLayout
            height: remoteUrlRowLayout.height
            spacing: remoteUrlRowLayout.spacing
            anchors.top: remoteUrlRowLayout.bottom
            anchors.left: remoteUrlRowLayout.left
            anchors.topMargin: 10
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
            Text {
                id: remotePortText2
                Layout.preferredHeight: localUrlComboBoxEx.height
                Layout.fillWidth: true
                text: remotePort
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: localUrlText.font.pixelSize
                color: localUrlText.color
            }
        }
    }

    function loadIpUrls(ipStringList) {
        localUrlComboBoxEx.resetModel(ipStringList)
    }

    function updateSocketConfig(senderIp, senderPort, recverPort) {
        remoteUrl = senderIp
        remotePort = senderPort
        localPort = recverPort
    }

}
