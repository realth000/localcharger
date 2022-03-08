import QtQuick 2.10

Rectangle {
    id: mainRect

    property int leftPadding: 5
    property int rightPadding: leftPadding
    property int topPadding: 5
    property int bottomPadding: topPadding
    property int spacing: 5
    property color chunkColor: "#336666"
    property real value: 0.4
    property color textColor: "#f0ffff"
    property string text: ""
    color: "transparent"
    height: 200
//    border.color: "#00ff00"

    SeparatorEx {
        id: sepTop
        height: 1
        sepColor: "#336666"
        anchors.top: mainRect.top
        anchors.topMargin: mainRect.topPadding
        anchors.left: mainRect.left
        anchors.leftMargin: mainRect.leftPadding
    }

    ButtonEx {
        id: statusIcon
        useTexts: false
        useIcon: true
        enablePressWave: false
        enabled: false
        bgColor: "transparent"
        iconUnchecked: "qrc:/pic/disconnected.png"
        anchors.top: sepTop.bottom
        anchors.topMargin: mainRect.topPadding
        anchors.left: mainRect.left
        anchors.leftMargin: mainRect.leftPadding
        width: 30
        height: 30
    }

    TextFieldEx {
        id: label
        text: mainRect.text
        color: textColor
        enableNativeBorder: true
        textLeftPadding: 0
        height: 30
        anchors.top: sepTop.bottom
        anchors.topMargin: mainRect.topPadding
        anchors.left: statusIcon.right
        anchors.leftMargin: 5
        anchors.right: mainRect.right
        anchors.rightMargin: mainRect.rightPadding
        background: Rectangle {
            color: "transparent"
//            border.color: "#ff0000"
        }
        enabled: false
    }

    Rectangle {
        id: chunk
        anchors.top: label.bottom
        anchors.topMargin: mainRect.spacing
        anchors.left: statusIcon.left
        anchors.right: label.right
        anchors.bottom: mainRect.bottom
        anchors.bottomMargin: mainRect.bottomPadding
        color: "transparent"
        border.width: 1
        border.color: chunkColor

        Rectangle {
            z: 1
            id: chunkFilled
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: Math.min(mainRect.value, 1) * chunk.width
            color: chunkColor
        }

        Text {
            text: (100*value).toFixed(2) + "%"
            font.pixelSize: 15
            anchors.horizontalCenter: chunk.horizontalCenter
            anchors.verticalCenter: chunk.verticalCenter
            z: 1
            color: "#f0ffff"
        }
    }

//    SeparatorEx {
//        id: sepDown
//        height: 1
//        sepColor: "#336666"
//        anchors.top: chunk.bottom
//        anchors.topMargin: mainRect.topPadding
//        anchors.left: mainRect.left
//        anchors.leftMargin: mainRect.leftPadding
//    }

    function setIcon(iconPath) {
        statusIcon.iconUnchecked = iconPath
    }

    function clearValue() {
        mainRect.value = 0
    }
}

