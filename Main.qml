import QtQuick
import QtQuick.Window
import QtCharts
import QtQuick.Controls.FluentWinUI3
import Odizinne.Ping

ApplicationWindow {
    id: root
    width: 500
    height: 300
    visible: true
    title: "Lagometer"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    opacity: 0

    property var pingData: []
    property int maxDataPoints: 60
    property double maxPingValue: 100

    // Fade animations for window
    NumberAnimation {
        id: fadeIn
        target: root
        property: "opacity"
        from: 0.0
        to: 1
        duration: 250
        easing.type: Easing.OutQuad
    }

    NumberAnimation {
        id: fadeOut
        target: root
        property: "opacity"
        from: 1
        to: 0.0
        duration: 250
        easing.type: Easing.InQuad
        onFinished: {
            // Actually hide the window when fade completes
            root.visible = false
        }
    }

    function performPing() {
        let pingTime = PingService.lastPingTime
        if (pingTime < 0) {
            pingTime = 0
        }

        // Update data array
        pingData.push(pingTime)
        if (pingData.length > maxDataPoints) {
            pingData.shift()
        }

        // Simple and reliable approach: clear and rebuild the series
        updateChart()
    }

    function updateChart() {
        // Clear existing data
        pingLineSeries.clear()

        // Calculate current max value for proper scaling
        let currentMax = 0
        for (let i = 0; i < pingData.length; i++) {
            if (pingData[i] > currentMax) {
                currentMax = pingData[i]
            }
        }

        // Set axis with some headroom (20%)
        maxPingValue = currentMax > 0 ? currentMax * 1.2 : 100
        axisY.max = maxPingValue

        // Rebuild series with correct indices
        for (let i = 0; i < pingData.length; i++) {
            pingLineSeries.append(i, pingData[i])
        }

        // Make sure X axis is properly scaled
        axisX.min = 0
        axisX.max = maxDataPoints - 1
    }

    function toggleVisibility() {
        if (root.visible) {
            // Start fade out animation
            fadeOut.start()
            // The window will be hidden when animation finishes
        } else {
            // Show window but with 0 opacity
            root.opacity = 0.0
            root.visible = true
            root.raise()
            root.requestActivate()

            // Position at top right
            positionTopRight()

            // Start fade in animation
            fadeIn.start()
        }
    }

    // Function to position the window at the top right
    function positionTopRight() {
        // Get the current screen
        let currentScreen = Qt.application.screens[0]  // Primary screen

        // Calculate position (top right with 15px margin)
        let xPos = currentScreen.width - root.width - 15
        let yPos = 15

        // Set window position
        root.x = xPos
        root.y = yPos
    }

    Connections {
        target: globalShortcut
        function onActivated() {
            toggleVisibility()
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
        // Start ping service
        PingService.targetHost = "8.8.8.8"
        PingService.startPinging()

        // Position window at top right on startup
        positionTopRight()

        // Start with fade in
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
}
