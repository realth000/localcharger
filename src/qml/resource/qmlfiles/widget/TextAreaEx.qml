import QtQuick 2.0
import QtQuick.Controls 2.5

TextArea {
    property color backgroundColor: "transparent"
    property int borderWidth: 1
    property color borderColor: "#336666"
    property color textColor: "#f0ffff"
    property int fontSize: 16
    id: textAreaEx
    color: textColor
    font.pixelSize: fontSize
    background: Rectangle {
        id: backgroundRectangle
        color: backgroundColor
        border.width: borderWidth
        border.color: borderColor
    }
    leftPadding: 10
    rightPadding: 10
    wrapMode: Text.WrapAnywhere
    selectByMouse: true
    selectedTextColor: textColor
//    selectionColor: "#5d5e5f"
    selectionColor: "#336666"
}
