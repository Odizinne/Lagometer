import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts
import QtQuick
import Odizinne.Lagometer

ApplicationWindow {
    title: "Lagometer settings"
    visible: false
    width: 450
    minimumWidth: 450
    maximumWidth: 450
    height: mainLyt.implicitHeight + 30
    minimumHeight: mainLyt.implicitHeight + 30
    maximumHeight: mainLyt.implicitHeight + 30
    transientParent: null

    ColumnLayout {
        id: mainLyt
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        property int labelWidth: Math.max(opacityLabel.implicitWidth, sizeLabel.implicitWidth)

        Label {
            text: "Flyout settings"
            font.pixelSize: 16
            Layout.leftMargin: 5
            Layout.topMargin: 10
            font.bold: true
        }

        Frame {
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                spacing: 15

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Opacity"
                        id: opacityLabel
                        Layout.preferredWidth: mainLyt.labelWidth
                    }

                    Slider {
                        from: 0.5
                        to: 1
                        value: UserSettings.opacity
                        Layout.rightMargin: -10
                        Layout.fillWidth: true
                        onValueChanged: {
                            UserSettings.opacity = value
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Size"
                        id: sizeLabel
                        Layout.preferredWidth: mainLyt.labelWidth
                    }

                    Slider {
                        from: 0.5
                        to: 1.5
                        value: UserSettings.windowScale
                        Layout.rightMargin: -10
                        Layout.fillWidth: true
                        onValueChanged: {
                            UserSettings.windowScale = value
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Position"
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
                        text: "Color"
                        Layout.fillWidth: true
                    }

                    ComboBox {
                        model: ["Blue", "Red", "Green", "Purple", "Pink", "Yellow", "Orange"]
                        currentIndex: UserSettings.colorIndex
                        onActivated: UserSettings.colorIndex = currentIndex
                    }
                }
            }
        }

        Label {
            text: "General settings"
            font.pixelSize: 16
            Layout.leftMargin: 5
            Layout.topMargin: 10
            font.bold: true
        }

        Frame {
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                spacing: 15
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Run at system startup"
                        Layout.fillWidth: true
                    }

                    Switch {
                        onClicked: {
                            lagometer.manageShortcut(checked)
                        }
                        Component.onCompleted: {
                            checked = lagometer.isShortcutPresent()
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Show flyout on app startup"
                        Layout.fillWidth: true
                    }

                    Switch {
                        onClicked: {
                            UserSettings.showFlyoutOnStartup = checked
                        }
                        Component.onCompleted: {
                            checked = UserSettings.showFlyoutOnStartup
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Flyout shortcut (Ctrl+Alt+Key)"
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: shortcutField
                        maximumLength: 1
                        placeholderText: "Key"
                        text: UserSettings.shortcutKey
                        implicitWidth: 50
                        horizontalAlignment: TextInput.AlignHCenter

                        onTextChanged: {
                            if (text.length === 1) {
                                let upperKey = text.toUpperCase()
                                text = upperKey
                                UserSettings.shortcutKey = upperKey
                                globalShortcut.updateShortcut(upperKey)
                            }
                        }

                        Component.onCompleted: {
                            text = UserSettings.shortcutKey
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    Label {
                        text: "Target host (IP or URL)"
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: targetHostField
                        placeholderText: "8.8.8.8"
                        text: UserSettings.targetHost
                        implicitWidth: 150

                        onEditingFinished: {
                            if (text.trim() !== "") {
                                UserSettings.targetHost = text.trim()
                            }
                        }
                    }
                }
            }
        }
    }
}
