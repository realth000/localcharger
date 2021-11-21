import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "widget"

Item {
    property QtObject root: mainConfigPageItem
    id: mainConfigPageItem
    Rectangle {
        anchors.fill: parent
        color: "transparent"
    }
    ColumnLayout {
        width: mainConfigPageItem.parent.width
        height: mainConfigPageItem.parent.height
        anchors.fill: parent
        spacing: 0
    }
    RowLayout {
        width: root.width
        height: 40
        spacing: 10
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

//        TextInputEx {
//            id:localUrlTextInputEx
//            height: parent.height
//            Layout.fillWidth: true
//            Layout.preferredHeight: parent.height
//            readOnly: true
//        }
    }
    function loadIpUrls(ipStringList){
        localUrlComboBoxEx.resetModel(ipStringList)
    }

}
