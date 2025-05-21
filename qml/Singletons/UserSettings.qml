pragma Singleton

import QtCore
import QtQuick

Settings {
    property real opacity: 1
    property int positionIndex: 0
    property int colorIndex: 0
    property bool showFlyoutOnStartup: true
    property real windowScale: 1.0
    property string shortcutKey: "L"
}
