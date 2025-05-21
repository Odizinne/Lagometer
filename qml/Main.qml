import QtQuick
import QtQuick.Window
import QtCharts
import QtQuick.Controls.Universal
import Odizinne.Ping
import Odizinne.Lagometer

ApplicationWindow {
    id: root
    width: 500
    height: 300
    visible: true
    title: "Lagometer"
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    opacity: UserSettings.opacity

    property var pingData: []
    property int maxDataPoints: 60
    property double maxPingValue: 100

    NumberAnimation {
        id: fadeIn
        target: root
        property: "opacity"
        from: 0
        to: UserSettings.opacity
        duration: 250
        easing.type: Easing.OutQuad
    }

    NumberAnimation {
        id: fadeOut
        target: root
        property: "opacity"
        from: UserSettings.opacity
        to: 0
        duration: 250
        easing.type: Easing.InQuad
        onFinished: {
            root.visible = false
        }
    }

    onVisibleChanged: {
        lagometer.windowVisible = visible
    }

    Connections {
        target: UserSettings
        function onOpacityChanged() {
            root.opacity = UserSettings.opacity
        }
    }

    Connections {
        target: lagometer

        function onRequestShowWindow() {
            if (!root.visible) {
                showFlyout()
            }
        }

        function onRequestHideWindow() {
            if (root.visible) {
                hideFlyout()
            }
        }

        function onRequestSettingsWindow() {
            settingsWindow.visible = true
        }
    }

    function performPing() {
        let pingTime = PingService.lastPingTime
        if (pingTime < 0) {
            pingTime = 0
        }

        pingData.push(pingTime)
        if (pingData.length > maxDataPoints) {
            pingData.shift()
        }

        updateChart()
    }

    function updateChart() {
        pingLineSeries.clear()
        let currentMax = 0
        for (let i = 0; i < pingData.length; i++) {
            if (pingData[i] > currentMax) {
                currentMax = pingData[i]
            }
        }

        maxPingValue = currentMax > 0 ? currentMax * 1.2 : 100
        axisY.max = maxPingValue

        for (let i = 0; i < pingData.length; i++) {
            pingLineSeries.append(i, pingData[i])
        }

        axisX.min = 0
        axisX.max = maxDataPoints - 1
    }

    function showFlyout() {
        root.opacity = 0.0
        root.visible = true
        root.raise()
        root.requestActivate()
        positionTopRight()
        fadeIn.start()
    }

    function hideFlyout() {
        fadeOut.start()
    }

    function positionTopRight() {
        let currentScreen = Qt.application.screens[0]

        let xPos = currentScreen.width - root.width - 15
        let yPos = 15

        root.x = xPos
        root.y = yPos
    }

    Connections {
        target: globalShortcut
        function onActivated() {
            if (root.visible)
                hideFlyout()
            else
                showFlyout()
        }
    }

    Connections {
        target: PingService
        function onLastPingTimeChanged() {
            performPing()
        }
        function onPingError(errorMessage) {
            console.error("Ping error:", errorMessage)
        }
    }

    Component.onCompleted: {
        PingService.targetHost = "8.8.8.8"
        PingService.startPinging()

        positionTopRight()

        root.opacity = 0.0
        fadeIn.start()
    }

    Component.onDestruction: {
        PingService.stopPinging()
    }

    ChartView {
        id: chartView
        anchors.fill: parent
        antialiasing: true
        backgroundColor: "transparent"
        legend.visible: false

        Rectangle {
            id: customBackground
            anchors.fill: parent
            color: "#282828"
            radius: 12
            z: -1
            border.width: 1
            border.color: "#777777"
        }

        ValueAxis {
            id: axisX
            visible: false
        }

        ValueAxis {
            id: axisY
            visible: true
            labelFormat: "%d ms"
            labelsColor: "#FFFFFF"
            labelsFont.pixelSize: 11
            labelsFont.bold: true
            gridLineColor: "#888888"
            minorGridLineColor: "#888888"
        }

        AreaSeries {
            id: pingAreaSeries
            axisX: axisX
            axisY: axisY
            borderWidth: 2
            borderColor: "#00AAFF"
            color: Qt.rgba(0, 0.67, 1, 0.3)
            opacity: 0.8

            upperSeries: LineSeries {
                id: pingLineSeries
                useOpenGL: true
                pointsVisible: false
                capStyle: Qt.RoundCap
                width: 2.5
            }
        }
    }

    SettingsWindow {
        id: settingsWindow
    }
}
