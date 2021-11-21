import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15

ComboBox {
    id: comboBoxEx
    editable: false
    font.pixelSize: 16
    model: ListModel {
        id: model
        ListElement {text: "s"}
    }
    background: Rectangle {
        color: "transparent"
        border.width: 1
        border.color: comboBoxEx.activeFocus ? "#f0ffff" : "#336666"
    }

    contentItem: Text {
        text:comboBoxEx.displayText
        font: comboBoxEx.font
        color: "#ff0000"
        leftPadding: 10;
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    popup: Popup {
        y: comboBoxEx.height + 1
        width: comboBoxEx.width
        height: comboBoxEx.height*5
        padding: 1
        contentItem: ListView{
            clip: true
            height: comboBoxEx.height
            width: comboBoxEx.width*5
        }

        background: Rectangle {
            color: "transparent"
            border.color: "#336666"
        }
    }


    function resetModel(elementList){
        console.log("reset model")
        for(let element of elementList){
            console.log(element)
            console.log(typeof(element))
            if (find(element) === -1){
                model.append({text: "123"})
            }
        }
    }
}
