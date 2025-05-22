import QtQuick
import QtQuick.Window
import QtCharts
import QtQuick.Controls.FluentWinUI3
import Odizinne.Ping
import Odizinne.Lagometer

ApplicationWindow {
    id: root
    property int baseWidth: 500
    property int baseHeight: 300

    width: baseWidth * UserSettings.windowScale
    height: baseHeight * UserSettings.windowScale
    visible: true
    title: "Lagometer"
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    opacity: UserSettings.opacity

    property var pingData: []
    property var pendingPingData: []
    property int maxDataPoints: 60
    property double maxPingValue: 100
    property bool isAnimating: false
    property int animationSteps: 60 // 60 steps for smooth animation
    property int currentAnimationStep: 0

    // Animation timer for continuous line drawing
    Timer {
        id: animationTimer
        interval: 1000 / animationSteps // ~16ms for 60fps
        repeat: true
        onTriggered: {
            if (currentAnimationStep < animationSteps && pingData.length > 0) {
                updateChartWithAnimation()
                currentAnimationStep++
            } else {
                stop()
                isAnimating = false
                currentAnimationStep = 0

                // If we have more pending data, start next animation
                if (pendingPingData.length > 0) {
                    startNextAnimation()
                }
            }
        }
    }

    NumberAnimation {
        id: fadeIn
        target: root
        property: "opacity"
        from: 0
        to: UserSettings.opacity
        duration: 250
        easing.type: Easing.InQuad
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

        function onPositionIndexChanged() {
            root.positionFlyout()
        }

        function onColorIndexChanged() {
            root.updateChartColors()
        }

        function onWindowScaleChanged() {
            root.width = root.baseWidth * UserSettings.windowScale
            root.height = root.baseHeight * UserSettings.windowScale

            positionFlyout()
        }

        function onTargetHostChanged() {
            PingService.targetHost = UserSettings.targetHost
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

        function onRequestClearChart() {
            clearChart()
        }
    }

    function clearChart() {
        pingData = []
        pendingPingData = []
        animationTimer.stop()
        isAnimating = false
        currentAnimationStep = 0

        pingLineSeries.clear()

        maxPingValue = 100
        axisY.max = maxPingValue
    }

    function performPing() {
        let pingTime = PingService.lastPingTime
        if (pingTime < 0) {
            pingTime = 0
        }

        // Add to pending data instead of immediate pingData
        pendingPingData.push(pingTime)

        // Start animation if not already running
        if (!isAnimating) {
            startNextAnimation()
        }
    }

    function startNextAnimation() {
        if (pendingPingData.length === 0) return

        // Move one data point from pending to actual data
        let newPoint = pendingPingData.shift()
        pingData.push(newPoint)

        if (pingData.length > maxDataPoints) {
            pingData.shift()
        }

        isAnimating = true
        currentAnimationStep = 0
        animationTimer.start()
    }

    function updateChartWithAnimation() {
        pingLineSeries.clear()

        // Calculate max value for scaling
        let currentMax = 0
        for (let i = 0; i < pingData.length; i++) {
            if (pingData[i] > currentMax) {
                currentMax = pingData[i]
            }
        }

        maxPingValue = currentMax > 0 ? currentMax * 1.2 : 100
        axisY.max = maxPingValue

        const totalSeconds = 60
        const pointSpacing = totalSeconds / (maxDataPoints - 1)

        // Calculate how many points to show based on animation progress
        if (pingData.length > 1) {
            // For the last segment, interpolate based on animation progress
            let progress = currentAnimationStep / animationSteps
            let lastCompletePoint = pingData.length - 2

            // Draw all complete points
            for (let i = 0; i <= lastCompletePoint; i++) {
                let timePoint = i * pointSpacing
                pingLineSeries.append(timePoint, pingData[i])
            }

            // Draw the animated segment
            if (lastCompletePoint >= 0) {
                let startValue = pingData[lastCompletePoint]
                let endValue = pingData[lastCompletePoint + 1]
                let currentValue = startValue + (endValue - startValue) * progress

                let startTime = lastCompletePoint * pointSpacing
                let endTime = (lastCompletePoint + 1) * pointSpacing
                let currentTime = startTime + (endTime - startTime) * progress

                pingLineSeries.append(currentTime, currentValue)
            }
        } else if (pingData.length === 1) {
            // First point - animate it appearing
            let progress = currentAnimationStep / animationSteps
            pingLineSeries.append(0, pingData[0] * progress)
        }
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

        const totalSeconds = 60
        const pointSpacing = totalSeconds / (maxDataPoints - 1)

        for (let i = 0; i < pingData.length; i++) {
            let timePoint = i * pointSpacing
            pingLineSeries.append(timePoint, pingData[i])
        }
    }

    function showFlyout() {
        root.opacity = 0.0
        root.visible = true
        root.raise()
        root.requestActivate()
        positionFlyout()
        fadeIn.start()
    }

    function hideFlyout() {
        fadeOut.start()
    }

    function getColorForIndex(index) {
        const colors = [
            { line: "#00AAFF", area: Qt.rgba(0, 0.67, 1, 0.3) },    // Blue
            { line: "#FF3333", area: Qt.rgba(1, 0.2, 0.2, 0.3) },   // Red
            { line: "#33CC33", area: Qt.rgba(0.2, 0.8, 0.2, 0.3) }, // Green
            { line: "#9933CC", area: Qt.rgba(0.6, 0.2, 0.8, 0.3) }, // Purple
            { line: "#FF66CC", area: Qt.rgba(1, 0.4, 0.8, 0.3) },   // Pink
            { line: "#FFCC00", area: Qt.rgba(1, 0.8, 0, 0.3) },     // Yellow
            { line: "#FF9933", area: Qt.rgba(1, 0.6, 0.2, 0.3) }    // Orange
        ]

        return colors[Math.min(index, colors.length - 1)]
    }

    function updateChartColors() {
        const colors = getColorForIndex(UserSettings.colorIndex)
        pingAreaSeries.borderColor = colors.line
        pingAreaSeries.color = colors.area
    }

    function positionFlyout() {
        let available = lagometer.availablePrimaryScreenGeometry()
        let margin = 15
        let xPos = 0
        let yPos = 0

        switch (UserSettings.positionIndex) {
        case 0: // Top right
            xPos = available.x + available.width - root.width - margin
            yPos = available.y + margin
            break
        case 1: // Top left
            xPos = available.x + margin
            yPos = available.y + margin
            break
        case 2: // Bottom right
            xPos = available.x + available.width - root.width - margin
            yPos = available.y + available.height - root.height - margin
            break
        case 3: // Bottom left
            xPos = available.x + margin
            yPos = available.y + available.height - root.height - margin
            break
        }

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
        PingService.targetHost = UserSettings.targetHost
        PingService.startPinging()

        positionFlyout()
        updateChartColors()

        root.opacity = 0.0
        if (UserSettings.showFlyoutOnStartup) {
            root.visible = true
            fadeIn.start()
        } else {
            root.visible = false
        }
    }

    Component.onDestruction: {
        PingService.stopPinging()
    }

    ChartView {
        id: chartView
        anchors.fill: parent
        antialiasing: true
        backgroundColor: "#313131"
        legend.visible: false
        backgroundRoundness: 12
        plotAreaColor: "#232323"

        Rectangle {
            id: plotBorder
            x: chartView.plotArea.x
            y: chartView.plotArea.y
            width: chartView.plotArea.width
            height: chartView.plotArea.height
            color: "transparent"
            border.width: 1
            border.color: "#888888"
            z: 1
        }

        ValueAxis {
            id: axisX
            visible: true
            labelsVisible: false
            labelsFont.pixelSize: 11
            gridLineColor: "#666666"
            minorGridLineColor: "#666666"
            min: 0
            max: 60
            tickCount: 5
            lineVisible: false
        }

        ValueAxis {
            id: axisY
            visible: true
            labelFormat: "%d ms  "
            labelsColor: "#FFFFFF"
            labelsFont.pixelSize: 11
            gridLineColor: "#666666"
            minorGridLineColor: "#666666"
            tickCount: 4
            lineVisible: false
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
