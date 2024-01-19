import QtQuick 2.15

Rectangle {
    anchors.fill: parent
    opacity: 0.1
    color: "red"

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        x: mouse.mouseX - 5
        y: mouse.mouseY - 5
        width: 10
        height: 10
        color: "blue"
    }
}
