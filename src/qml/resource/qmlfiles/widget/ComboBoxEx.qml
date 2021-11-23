import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQml.Models 2.10

ComboBox {
    property color backgroundColor: "#2d2e30"
    property color itemSelectedBackgroundColor: "#254158"
    property color borderColor: "#336666"
    property int itemHeight: 30
    property color textSelectedColor: textColor
    property color textColor: "#f0ffff"
    property int itemMaxShowCount: 100
    property int textLeftPadding: 10
    property int arrowHalfSize: 3
    property color arrowColor: "#f0ffff"

    id: comboBoxEx
    editable: false
    font.pixelSize: 16
    model: ListModel {
        id: model
//        ListElement {text: "s"}
    }

    // current item
    contentItem: TextFieldEx {
        id: currentItemTextFieldEx
        readOnly: !comboBoxEx.editable
        text: comboBoxEx.displayText === "" && comboBoxEx.delegateModel.count > 0 ?
                  comboBoxEx.delegateModel.model.get(0).text : comboBoxEx.displayText
        font: comboBoxEx.font
        leftPadding: textLeftPadding
        enableNativeBorder: true
    }


    // item in ListView
    delegate: ItemDelegate {
        id: delegate
        height: comboBoxEx.height
        width: parent.width
        padding: 0
        contentItem: Text {
            id: itemText
            text: modelData
            leftPadding: textLeftPadding
            color: index === itemView.currentIndex ? textSelectedColor : textColor
            // set font to display
            font: comboBoxEx.font
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: index === itemView.currentIndex ? itemSelectedBackgroundColor : backgroundColor
            border.color: borderColor
            //item底部的线
            Rectangle {
                height: 1
                width: parent.width
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                color: borderColor
            }
        }
    }

    background: Rectangle {
        color: backgroundColor
        border.width: 1
        border.color: borderColor
    }

    // list view
    popup: Popup {
        y: comboBoxEx.height + 1
        width: comboBoxEx.width
        height: (comboBoxEx.height + padding )*(Math.min(itemMaxShowCount, comboBoxEx.delegateModel.count))
        padding: 0
        contentItem: ListView {
            id: itemView
            clip: true
            height: comboBoxEx.contentHeight
            width: comboBoxEx.width
            model: comboBoxEx.delegateModel
            currentIndex: comboBoxEx.highlightedIndex
        }
    }

    indicator: Item {
        id: arrow
        width: height
        height: comboBoxEx.height
        anchors.top: comboBoxEx.top
        anchors.right: comboBoxEx.right

        Canvas {
            id: canvas
            anchors.fill: parent
            onPaint: {
                var cvs = getContext("2d")
                cvs.lineWidth = 2
                cvs.strokeStyle = arrowColor
                cvs.fillStyle = arrowColor

                cvs.beginPath()
                cvs.moveTo(width*0.5 - arrowHalfSize, height*0.5 - arrowHalfSize)
                cvs.lineTo(width*0.5                , height*0.5 + arrowHalfSize)
                cvs.lineTo(width*0.5 + arrowHalfSize, height*0.5 - arrowHalfSize)
                cvs.closePath()

                cvs.fill()
                cvs.stroke()
            }
        }


    }




    function resetModel(elementList) {
        for(let element of elementList){
            if (find(element) === -1){
                model.append({text: element})
            }
        }
    }
}
