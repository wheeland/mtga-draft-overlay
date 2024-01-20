import QtQuick 2.15

Item {
    id: root
    anchors.fill: parent

    property bool draftInfoEnabled: false
    property bool showDraftInfo: root.draftInfoEnabled && !_inputTracker.alt

    Column {
        id: controlsRow
        spacing: 5

        Rectangle {
            color: "darkred"
            width: 30
            height: 20

            Text {
                anchors.centerIn: parent
                text: "exit"
                color: "white"
                font.pixelSize: 12.0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: _app.quit()
            }
        }

        Rectangle {
            color: "darkgray"
            width: 30
            height: 20

            Text {
                anchors.centerIn: parent
                text: "clear\ncache"
                color: "white"
                font.pixelSize: 8.0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: _cardDB.clearCache()
            }
        }

        Rectangle {
            color: root.showDraftInfo ? "green" : "red"
            width: 30
            height: 20

            Text {
                anchors.centerIn: parent
                text: "show"
                color: "white"
                font.pixelSize: 12.0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.draftInfoEnabled = !root.draftInfoEnabled
            }
        }
    }

    Item {
        anchors.fill: parent

        Repeater {
            model: _draftModel
            delegate: Component {
                Item {
                    visible: root.showDraftInfo
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
                            text: model.name
                            color: model.color
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }
    }
}
