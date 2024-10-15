import QtQuick 2.15

Item {
    id: root

    property real size: 20
    property int color: 0

    width: root.size
    height: root.size

    Rectangle {
        anchors.fill: parent
        radius: root.size / 2
        color: "gray"
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: root.size / 8
        radius: width / 2
        color: ["#f0f2c0", "#90a5d0", "#8c828a", "#db8664", "#85b078"][root.color]
    }

}
