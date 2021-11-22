import QtQuick 2.0
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10

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
