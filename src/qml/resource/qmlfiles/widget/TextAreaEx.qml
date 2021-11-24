import QtQuick 2.0
import QtQuick.Controls 2.5

TextArea {
    property color backgroundColor: "transparent"
    property int borderWidth: 1
    property color borderColor: "#336666"
    id: textAreaEx
    background: Rectangle {
        id: backgroundRectangle
        color: backgroundColor
        border.width: borderWidth
        border.color: borderColor
    }
}
