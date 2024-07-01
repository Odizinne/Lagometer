import sys
import ping3
import time
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QTimer

class PingDisplayApp(QtWidgets.QMainWindow):
    def __init__(self):
        super(PingDisplayApp, self).__init__()
        uic.loadUi('design.ui', self)

        self.setFixedSize(self.size())
        self.setWindowTitle('Lagometer')

        self.sliders = [
            self.verticalSlider, self.verticalSlider_2, self.verticalSlider_3, self.verticalSlider_4,
            self.verticalSlider_5, self.verticalSlider_6, self.verticalSlider_7, self.verticalSlider_8,
            self.verticalSlider_9, self.verticalSlider_10
        ]

        for slider in self.sliders:
            slider.setMaximum(200)

        self.pingLabel = self.findChild(QtWidgets.QLabel, 'pingLabel')

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_ping)
        self.timer.start(1000)

        self.update_count = 0

    def update_ping(self):
        timeout = 1
        start_time = time.time()

        ping_value = None

        while (time.time() - start_time) < timeout:
            ping = ping3.ping('google.com', unit='ms')

            if ping is None or ping is False:
                ping_value = 200
                self.pingLabel.setText("Connection lost")
                break
            elif ping == 0.0:
                continue
            else:
                ping_value = round(ping)
                self.pingLabel.setText(f"Ping: {ping_value} ms")
                break
            
        slider_value = min(ping_value, 200) if ping_value is not None else 200

        if self.update_count < 10:
            self.sliders[self.update_count].setValue(slider_value)
            self.update_count += 1
        else:
            for i in range(9):
                self.sliders[i].setValue(self.sliders[i + 1].value())
            self.sliders[9].setValue(slider_value)

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = PingDisplayApp()
    window.show()
    sys.exit(app.exec())
