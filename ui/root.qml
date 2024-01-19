import QtQuick 2.15

Item {
    anchors.fill: parent

    Rectangle {
        id: clicker
        color: "red"
        x: 0
        y: 0
        width: 20
        height: 20

        Text {
            anchors.centerIn: parent
            text: "x"
            color: "black"
        }
    }

    Repeater {
        model: _draftModel
        delegate: Component {
            Rectangle {
                x:  _layout.cardArtCenter(model.index).x - width / 2
                y:  _layout.cardArtCenter(model.index).y - height/ 2
                width: 80
                height: 80
                color: "black"
                opacity: 0.7

                Column {
                    padding: 5
                    spacing: 5
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Pick: " + model.avgPick.toFixed(1)
                        color: "white"
                        font.pixelSize: 12
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "ALSA: " + model.avgSeen.toFixed(1)
                        color: "white"
                        font.pixelSize: 12
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Win: " + (100 * model.winRate).toFixed(1) + "%"
                        color: "white"
                        font.pixelSize: 12
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Card ID: " + model.id
                        color: "white"
                        font.pixelSize: 10
                    }
                }
            }
        }
    }
}
