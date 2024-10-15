import QtQuick 2.15

Row {
    id: root

    property real size: 20
    property int colors: 0

    width: childrenRect.width
    height: size

    Repeater {
        model: 5
        ColorBlob {
            size: root.size
            color: model.index
            visible: root.colors & (1 << model.index)
        }
    }
}
