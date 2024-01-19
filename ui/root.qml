import QtQuick 2.15

Item {
    anchors.fill: parent

    Rectangle {
        color: "darkred"
        x: 0
        y: 0
        width: 30
        height: 20

        Text {
            anchors.centerIn: parent
            text: "EXIT"
            color: "white"
            font.pixelSize: 12.0
        }

        MouseArea {
            anchors.fill: parent
            onClicked: _app.quit()
        }
    }

    Repeater {
        model: _draftModel
        delegate: Component {
            Item {
                x:  _layout.cardArtCenter(model.index).x - width / 2
                y:  _layout.cardArtCenter(model.index).y - height/ 2
                width: 80
                height: 80

                Rectangle {
                    anchors.fill: parent
                    color: "black"
                    opacity: 0.7
                }

                Column {
                    anchors.fill: parent
                    padding: 3
                    spacing: 3
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Pick: " + model.avgPick.toFixed(1)
                        color: _layout.draftValueColor((model.avgPick - 1) / 12.0)
                        font.pixelSize: 14
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "ALSA: " + model.avgSeen.toFixed(1)
                        color: _layout.draftValueColor((model.avgSeen - 1) / 9.0)
                        font.pixelSize: 14
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Win: " + (100 * model.winRate).toFixed(1) + "%"
                        color: _layout.draftValueColor(0.5 + (model.winRate - 0.55) * 10.0)
                        font.pixelSize: 14
                    }
                }
            }
        }
    }
}
