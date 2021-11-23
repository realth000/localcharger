import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: gbexMainRect
    property QtObject labelRect: gbexLabel
    property QtObject separator: gbexSeparater
    readonly property int titleHeight: labelHeight + gbexSeparater.height
    property string iconPath
    property string labelText
    property color labelColor: "#4b6876"
    property color labelBgColor: "transparent"
    property int labelWidth: 200
    property int labelHeight: 60
    property int labelTextSize: 16
    property bool labelTextBold: true
    property int labelLeftMargin: 15
    property int separatorLeftMargin: 15
    property int separatorRightMargin: 15
    property color separatorColor: labelColor
    width: parent.width
    height: 200
    color: "transparent"

    Rectangle {
        id: gbexBackgroundRect
        width: parent.width
        height: parent.height
        color: "transparent"
    }
    ButtonEx {
        id: gbexLabel
        width: gbexBackgroundRect.width
        height: labelHeight
        checkable: false
        iconUnchecked: iconPath
        iconPos: 1
        bgColor: labelBgColor
        anchors.left: gbexBackgroundRect.left
        anchors.leftMargin: labelLeftMargin
        anchors.top: gbexBackgroundRect.top
        texts: labelText
        textsUncheckedColor: labelColor
        textsSize: labelTextSize
        textsBold: labelTextBold
        posToLeft:true
    }

    SeparatorEx {
        id: gbexSeparater
        width: gbexBackgroundRect.width - separatorLeftMargin - separatorRightMargin
        height: 1
        anchors.top: gbexLabel.bottom
        anchors.horizontalCenter: gbexBackgroundRect.horizontalCenter
        anchors.leftMargin: separatorLeftMargin
        anchors.rightMargin: separatorRightMargin
        color: separatorColor
        border.color: separatorColor
    }
}
