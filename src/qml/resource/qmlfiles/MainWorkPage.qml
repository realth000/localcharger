import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: mainWorkPageItem
    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }
    ColumnLayout {
        width: mainWorkPageItem.parent.width
        height: mainWorkPageItem.parent.height
        anchors.fill: parent
        spacing: 0
    }

}
