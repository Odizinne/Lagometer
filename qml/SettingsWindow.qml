import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts
import QtQuick
import Odizinne.Lagometer

ApplicationWindow {
    title: "Lagometer settings"
    visible: false
    width: 400
    height: 600

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            Label {
                text: "Flyout opacity"
                Layout.fillWidth: true
            }

            Slider {
                from: 0
                to: 1
                value: UserSettings.opacity
                onValueChanged: UserSettings.opacity = value
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            Label {
                text: "Flyout Position"
                Layout.fillWidth: true
            }

            ComboBox {
                model: ["Top right", "Top left", "Bottom right", "Bottom left"]
                currentIndex: UserSettings.positionIndex
                onActivated: UserSettings.positionIndex = currentIndex
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            Label {
                text: "Flyout color"
                Layout.fillWidth: true
            }

            ComboBox {
                model: ["Blue", "Red", "Green", "Purple", "Pink", "Yellow", "Orange"]
                currentIndex: UserSettings.colorIndex
                onActivated: UserSettings.colorIndex = currentIndex
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            Label {
                text: "Run at system startup"
                Layout.fillWidth: true
            }

            CheckBox {
                onClicked: {

                }
            }
        }
    }
}
