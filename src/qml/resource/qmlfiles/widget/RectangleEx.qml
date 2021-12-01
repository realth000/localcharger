import QtQuick 2.10

Rectangle {
    property bool commonBorderColor : true
    property int topBorderwidth : 1
    property int bottomBorderwidth : 1
    property int leftBorderwidth : 1
    property int rightBorderwidth : 1
    property color topBorderColor: "transparent"
    property color bottomBorderColor: "transparent"
    property color leftBorderColor: "transparent"
    property color rightBorderColor: "transparent"
    property int commonBorderWidth : 1
    property color borderColor : "transparent"

    z : parent.z

    Rectangle {
        id: topBorder
        width: parent.width
        height: topBorderwidth
        z : parent.z + 1
        anchors.verticalCenter: parent.top
        anchors.verticalCenterOffset: topBorderwidth
        anchors.horizontalCenter: parent.horizontalCenter
        color: commonBorderColor ? parent.borderColor : topBorderColor
        border.color: color
    }
    Rectangle {
        id: bottomBorder
        width: parent.width
        height: bottomBorderwidth
        z : parent.z + 1
        anchors.verticalCenter: parent.bottom
        anchors.verticalCenterOffset: -bottomBorderwidth
        anchors.horizontalCenter: parent.horizontalCenter
        color: commonBorderColor ? parent.borderColor : bottomBorderColor
        border.color: color
    }
    Rectangle {
        id: leftBorder
        width: leftBorderwidth
        height: parent.height
        z : parent.z + 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.left
        anchors.horizontalCenterOffset: leftBorderwidth
        color: commonBorderColor ? parent.borderColor : leftBorderColor
        border.color: color
    }
    Rectangle {
        id: rightBorder
        width: rightBorderwidth
        height: parent.height
        z : parent.z + 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.right
//        anchors.horizontalCenterOffset: -rightBorderwidth
        color: commonBorderColor ? parent.borderColor : rightBorderColor
        border.color: color
    }
}
