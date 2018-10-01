import serial
import re
import random
from threading import Thread
import sys
import math 
from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.Qt import QPushButton, QPainter, QColor, Qt,\
    QVBoxLayout, QHBoxLayout, QGroupBox, \
    QSizePolicy, QTimer, QPen, QSpinBox, QGridLayout, QLabel, \
    QDoubleSpinBox, QLineEdit, QMainWindow, QFont
 
s = serial.Serial(port="COM3", baudrate=115200)
thread_done = True
 
def com_port_reader():
    while not thread_done:
        #print("reading from serial port")
        try:
            line = s.readline()
            #for l in line:
            #    print(l, end=' ')
            line = line.decode('utf-8')
            print(line, end='')
              
            m = re.match('(\d+\.\d+)[ ]+(\d+)', line)
            if m:
                angle = float(m.group(1))
                half_steps = int(m.group(2))
                #print("found match")
                tracker.set_current_angle_in_degrees(angle)
                tracker.show_half_steps(half_steps)
              
        except Exception as e:
            print(e)

class Point(object):
    def __init__(self, x=0, y=0, text=""):
        self.x = x
        self.y = y
        self.text = text
    
    def __str__(self):
        return 'x=%.2f y=%.2f' % (self.x, self.y)


class MainWindow(QMainWindow):
    def __init__(self):
        pass

class ShadowTipTracker(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
     
    def initUI(self):
        
        self.current_height = 0
        self.time_paused = False
        self.x_axis_y_coordinate = 248
        self.timer_interval = 100
        self.amplitude = 220
        self.pen_width = 10
        self.x_axis_offset_from_right = 523
        self.time_x_increment = 3               # num pixels everything moves left in each paint iteration  
#         self.angle = 0
        
        self.setMinimumHeight(400)
        self.setWindowTitle('Rotating vector')
        self.setSizePolicy( QSizePolicy.Expanding, QSizePolicy.Expanding)
        
        
        self.timer = QTimer()
        self.timer.timeout.connect(self.timerEvent)
        self.timer.start(self.timer_interval)
        
        random.seed(0)
        self.ordinates = [0] * 1000
        self.bkTextPoints = [Point(random.randint(0, 1900),
                                   random.randint(0, 800),
                                   text="Background") for _ in range(10)]
        self.timeTextPoints = [Point(random.randint(0, 1900),
                                     random.randint(0, 800),
                                     text="Time") for _ in range(10)]
        self.show()
        
        
    def set_amplitude(self, amplitude):
        self.amplitude = amplitude
    
    def _set_current_angle_in_radians(self, current_angle):
        """
        The current angle in radians is NOT stored in the object.
        """
        self.current_height = self.amplitude * math.sin(current_angle)
        #print("setting current height = " + str(self.current_height))
    
    def set_current_angle_in_degrees(self, current_angle_degrees):
        # Be 1 degree ahead of what arduino reports. This is so that the shadow and simulation look in sync.
        current_angle_degrees += 1
        
        self._set_current_angle_in_radians(
            (current_angle_degrees + runningAngleOffset_sb.value()) * math.pi / 180
        )
        global angle_le
        angle_le.setText(str(current_angle_degrees))
    
    def show_half_steps(self, half_steps):
        global halfSteps_le
        halfSteps_le.setText(str(half_steps))
    
    def set_timer_interval(self, interval):
        interval = interval * 10
#         print("setting timer interval " + str(interval))
        self.timer_interval = interval
        self.timer.start(self.timer_interval)
        
    def set_pen_width(self, width):
        self.pen_width = width
    
    def set_x_axis_y_coordinate(self, value):
        self.x_axis_y_coordinate = value
    
    def set_x_axis_offset_from_right(self, value):
        self.x_axis_offset_from_right = value
    
    def pause_time(self):
        self.time_paused = True
    def unpause_time(self):
        self.time_paused = False
        
    
    def paintEvent(self, event):
        qp = QPainter()
        qp.begin(self)
        self.draw(event, qp)
        qp.end()
     
    def draw(self, event, qp):
#         self.angle += 0.04
#         self._set_current_angle_in_radians(self.angle)

        #--------------------------------------------------------------------        
        # Draw X axis
        #--------------------------------------------------------------------        
        pen = QPen(QColor(120, 120, 120))
        pen.setWidth(2)
        pen.setCapStyle(Qt.RoundCap)
        qp.setPen(pen)
        qp.drawLine(0,
                    self.x_axis_y_coordinate,
                    self.width() - self.x_axis_offset_from_right,
                    self.x_axis_y_coordinate)
        
        #--------------------------------------------------------------------        
        # Draw rectangle that will have the vector's shadow
        #--------------------------------------------------------------------        
        pen = QPen(QColor(100, 100, 100))
        pen.setWidth(2)
        pen.setCapStyle(Qt.RoundCap)
        qp.setPen(pen)
        qp.setOpacity(0.2)
        qp.drawRect(self.width() - self.x_axis_offset_from_right - self.pen_width//2 - 2,
                    self.x_axis_y_coordinate - self.amplitude - self.pen_width//2 - 2,
                    self.pen_width + 2,
                    self.amplitude*2 + self.pen_width + 2)
        qp.setOpacity(1)

        
        #--------------------------------------------------------------------        
        # Draw background
        #--------------------------------------------------------------------        
        font = QFont()
        font.setPixelSize(30)
        pen = QPen(QColor(100, 100, 100))
        qp.setPen(pen)
        qp.setBrush(Qt.green)
        qp.setFont(font)
        qp.setOpacity(0.2)
        for p in self.bkTextPoints + self.timeTextPoints:
#             qp.rotate(45)
            qp.drawText(p.x, p.y, p.text)
#             qp.drawEllipse(p.x, p.y, 50, 30)
#             qp.rotate(-45)
        
            #--------------------------------------------------
            # If time isn't paused, shift all background objects left     
            if not self.time_paused:
                p.x -= self.time_x_increment
                if p.x <= -150:
                    p.x = 2000

        qp.setOpacity(1)
        
        #--------------------------------------------------------------------        
        # Draw points
        #--------------------------------------------------------------------        
        pen = QPen(QColor(120, 60, 60))
        pen.setWidth(self.pen_width)
        pen.setCapStyle(Qt.RoundCap)
        qp.setPen(pen)
#         qp.setBrush(QBrush(Qt.black))

        if not self.time_paused:
            # Copy height of each point to the point on its left (older point)
            for i in reversed(range(len(self.ordinates) - 1)):
                self.ordinates[i+1] = self.ordinates[i]
        
        #print("Using current height of " + str(self.current_height))
        self.ordinates[0] = -self.current_height            # set height of "current" (rightmost) sample
        
        for i in range(len(self.ordinates) - 1):
            qp.drawLine(self.width() - self.x_axis_offset_from_right - i*self.time_x_increment,
                        self.ordinates[i] + self.x_axis_y_coordinate,
                        self.width() - self.x_axis_offset_from_right - (i+1)*self.time_x_increment,
                        self.ordinates[i+1] + self.x_axis_y_coordinate)
            
    def timerEvent(self):
        self.repaint()

angle_le = None
halfSteps_le = None

def createMonitorControls():
    gb = QGroupBox("Monitor")
    
    #------------------------------------------

    layout = QHBoxLayout()
    
    label1 = QLabel("Angle:")
    label2 = QLabel("Half steps:")
    global angle_le
    angle_le = QLineEdit()
    global halfSteps_le
    halfSteps_le = QLineEdit()
    
    angle_le.setMaximumWidth(50)
    halfSteps_le.setMaximumWidth(50)
    
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(5)
    
    layout.addWidget(label1)
    layout.addWidget(angle_le)
    layout.addWidget(label2)
    layout.addWidget(halfSteps_le)
    layout.addStretch()
    
    upper = QWidget()
    upper.setLayout(layout)

    
    #------------------------------------------
    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addStretch(1)

    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)
    
    gb.setLayout(vbox)
    return gb

def createSpeedControls():
    speed_gb = QGroupBox("Speed")
    speed_gb.setMaximumWidth(50)
    
    layout = QVBoxLayout()
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)
    
         
    speed1_btn = QPushButton("1")
    speed2_btn = QPushButton("2")
    speed3_btn = QPushButton("3")
    speed4_btn = QPushButton("4")
    speed5_btn = QPushButton("5")
    speed6_btn = QPushButton("6")

    speed1_btn.clicked.connect(lambda: send_cmd(["1"]))
    speed2_btn.clicked.connect(lambda: send_cmd(["2"]))
    speed3_btn.clicked.connect(lambda: send_cmd(["3"]))
    speed4_btn.clicked.connect(lambda: send_cmd(["4"]))
    speed5_btn.clicked.connect(lambda: send_cmd(["5"]))
    speed6_btn.clicked.connect(lambda: send_cmd(["6"]))
    
    layout.addWidget(speed1_btn)
    layout.addWidget(speed2_btn)
    layout.addWidget(speed3_btn)
    layout.addWidget(speed4_btn)
    layout.addWidget(speed5_btn)
    layout.addWidget(speed6_btn)
    layout.addStretch(1)

    speed_gb.setLayout(layout)
    
    return speed_gb

def createCalibrationControls():
    gb = QGroupBox("Calibration")
    
    #------------------------------------------
    layout = QGridLayout()
    
    arduino = createArduinoCalibrationControls()
    pc = createPcCalibrationControls()
    gui = createGuiCalibration()
    
    layout.addWidget(arduino,   0, 0, 1, 1)
    layout.addWidget(pc,        1, 0, 1, 1)
    layout.addWidget(gui,       0, 1, 2, 1)
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(5)
    
    upper = QWidget()
    upper.setLayout(layout)
    
    #------------------------------------------
    
    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addStretch(1)

    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)
    
    gb.setLayout(vbox)
    return gb
    


offAt180_sb = None
runningAngleOffset_sb = None

def createPcCalibrationControls():
    gb = QGroupBox("PC Calibration")
    
    #-------------------------------------------

    layout = QHBoxLayout()
    
    label = QLabel("Calibration offset for 180:")
    global offAt180_sb
    offAt180_sb = QDoubleSpinBox()
    offAt180_sb.setRange(-4.0, 0.0)
    offAt180_sb.setSingleStep(0.5)
    offAt180_sb.setValue(-3.5)
    offAt180_sb.setMaximumWidth(60)
    
    layout.addWidget(label)
    layout.addWidget(offAt180_sb)
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    upper = QWidget()
    upper.setLayout(layout)

    #-------------------------------------------
    
    layout = QHBoxLayout()

    btn2p5 = QPushButton("2.5")
    btn2p5.clicked.connect(lambda: offAt180_sb.setValue(-2.5))

    btn3 = QPushButton("3.0")
    btn3.clicked.connect(lambda: offAt180_sb.setValue(-3.0))

    btn3p5 = QPushButton("3.5")
    btn3p5.clicked.connect(lambda: offAt180_sb.setValue(-3.5))
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    layout.addWidget(btn2p5)
    layout.addWidget(btn3)
    layout.addWidget(btn3p5)
    
    middle = QWidget()
    middle.setLayout(layout)

    #-------------------------------------------
    
    layout = QHBoxLayout()
    label = QLabel("Running Angle offset:")
    global runningAngleOffset_sb
    runningAngleOffset_sb = QSpinBox()

    runningAngleOffset_sb.setToolTip("offset in degrees to add to the angle reported by arduino")
    runningAngleOffset_sb.setRange(0, 5)
    runningAngleOffset_sb.setValue(2)
    runningAngleOffset_sb.setMaximumWidth(60)
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    layout.addWidget(label)
    layout.addWidget(runningAngleOffset_sb)
    
    lower = QWidget()
    lower.setLayout(layout)
    
    
    #-------------------------------------------
    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addWidget(middle)
    vbox.addWidget(lower)
    vbox.addStretch(1)

    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    gb.setLayout(vbox)
    return gb

def createArduinoCalibrationControls():
    gb = QGroupBox("Arduino Calibration")
    vbox = QVBoxLayout()

    #-------------------------------------------
    layout = QHBoxLayout()
    
    label = QLabel("Set current position as:")
    lineEdit = QLineEdit()
    degreesLabel = QLabel("deg")
    set_btn = QPushButton("Set")

    lineEdit.setMaximumWidth(30)
    set_btn.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)

    set_btn.clicked.connect(lambda: send_cmd(["=" + lineEdit.displayText()]))

    layout.setContentsMargins(0,0,0,0)

    layout.addWidget(label)
    layout.addWidget(lineEdit)
    layout.addWidget(degreesLabel)
    layout.addWidget(set_btn)

    upper = QWidget()
    upper.setLayout(layout)

    #-------------------------------------------

    layout = QHBoxLayout()

    btn0 = QPushButton("Set as 0")
    btn180 = QPushButton("Set as 180")
    
    btn0.clicked.connect(lambda: send_cmd(["=0"]))
    btn180.clicked.connect(lambda: send_cmd(["=180"]))

    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    layout.addWidget(btn0)
    layout.addWidget(btn180)
    layout.addStretch(1)

    lower = QWidget()
    lower.setLayout(layout)

    #-------------------------------------------

    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    vbox.addWidget(upper)
    vbox.addWidget(lower)
    gb.setLayout(vbox)
    return gb

def createGuiCalibration():
    gb = QGroupBox("Gui Calibration")
    
    layout = QGridLayout()
    
    label1 = QLabel("Time speed (delay):")
    label2 = QLabel("Sine amplitude:")
    label3 = QLabel("Pen width:")
    label4 = QLabel("Vertical position:")
    label5 = QLabel("X axis offset from right:")
    
    timeDelay_sb = QSpinBox()
    timeDelay_sb.setRange(2, 20)                            # actual ms is times 10
    timeDelay_sb.setValue(tracker.timer_interval / 10)      # value is in multiples of 10 ms
    timeDelay_sb.setMaximumWidth(60)
    timeDelay_sb.setMinimumHeight(25)
    timeDelay_sb.valueChanged.connect(tracker.set_timer_interval)
    
    amplitude_sb = QSpinBox()
    amplitude_sb.setRange(20, 300)
    amplitude_sb.setValue(tracker.amplitude)
    amplitude_sb.setMaximumWidth(60)
    amplitude_sb.setMinimumHeight(25)
    amplitude_sb.valueChanged.connect(tracker.set_amplitude)

    penWidth_sb = QSpinBox()
    penWidth_sb.setRange(1, 20)
    penWidth_sb.setValue(tracker.pen_width)
    penWidth_sb.setMaximumWidth(60)
    penWidth_sb.setMinimumHeight(25)
    penWidth_sb.valueChanged.connect(tracker.set_pen_width)

    vertPos_sb = QSpinBox()
    vertPos_sb.setRange(200, 800)
    vertPos_sb.setValue(tracker.x_axis_y_coordinate)
    vertPos_sb.setMaximumWidth(60)
    vertPos_sb.setMinimumHeight(25)
    vertPos_sb.valueChanged.connect(tracker.set_x_axis_y_coordinate)

    xAxisLength_sb = QSpinBox()
    xAxisLength_sb.setRange(50, 1000)
    xAxisLength_sb.setValue(tracker.x_axis_offset_from_right)
    xAxisLength_sb.setMaximumWidth(60)
    xAxisLength_sb.setMinimumHeight(25)
    xAxisLength_sb.valueChanged.connect(tracker.set_x_axis_offset_from_right)


    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    layout.addWidget(label1,            0, 0, 1, 1)
    layout.addWidget(timeDelay_sb,      0, 1, 1, 1)
    layout.addWidget(label2,            1, 0, 1, 1)
    layout.addWidget(amplitude_sb,      1, 1, 1, 1)
    layout.addWidget(label3,            2, 0, 1, 1)
    layout.addWidget(penWidth_sb,       2, 1, 1, 1)
    layout.addWidget(label4,            3, 0, 1, 1)
    layout.addWidget(vertPos_sb,        3, 1, 1, 1)
    layout.addWidget(label5,            4, 0, 1, 1)
    layout.addWidget(xAxisLength_sb,    4, 1, 1, 1)


    upper = QWidget()
    upper.setLayout(layout)

    #-------------------------------------------------
    
    vbox = QVBoxLayout()
    
    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    vbox.addWidget(upper)
    vbox.addStretch(1)
    
    gb.setLayout(vbox)
    return gb

def createNavigationControls():
    gb = QGroupBox("Navigation Controls")

    #-------------------------------------------

    layout = QGridLayout()
    
    pause_both_btn  = QPushButton("Pause vector && time")
    pause_btn       = QPushButton("Pause vector")
    pause_time_btn  = QPushButton("Pause time")
    continue_btn    = QPushButton("Continue")
    release_btn     = QPushButton("Release")
    cwHalfStep_btn  = QPushButton("=> half step")
    ccwHalfStep_btn = QPushButton("<= half step")
    ccwDir_btn      = QPushButton("CCW direction")
    cwDir_btn       = QPushButton("CW direction")
    
    continue_btn.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
    
    pause_both_btn.clicked.connect  (pause_vector_and_time)
    pause_btn.clicked.connect       (lambda: send_cmd(["p"]))
    pause_time_btn.clicked.connect  (tracker.pause_time)
    continue_btn.clicked.connect    (continue_)
    release_btn.clicked.connect     (lambda: send_cmd(["r"]))
    cwHalfStep_btn.clicked.connect  (lambda: send_cmd([">", "h", "<"]))
    ccwHalfStep_btn.clicked.connect (lambda: send_cmd(["<", "h"]))
    ccwDir_btn.clicked.connect      (lambda: send_cmd(["<"]))
    cwDir_btn.clicked.connect       (lambda: send_cmd([">"]))

    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)
    layout.setVerticalSpacing(0)

    layout.addWidget(pause_both_btn,        0, 0)
    layout.addWidget(pause_btn,             1, 0)
    layout.addWidget(pause_time_btn,        2, 0)
    layout.addWidget(continue_btn,          0, 1, 3, 1)
    layout.addWidget(ccwHalfStep_btn,       3, 0)
    layout.addWidget(cwHalfStep_btn,        3, 1)
    layout.addWidget(ccwDir_btn,            4, 0)
    layout.addWidget(cwDir_btn,             4, 1)
    layout.addWidget(createGotoControls(),  5, 0, 1, 2)
    layout.addWidget(release_btn,           6, 0, 1, 2)

    upper = QWidget()
    upper.setLayout(layout)
    
    #-------------------------------------------
    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addStretch(1)
    
    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    gb.setLayout(vbox)
    return gb

def createGotoControls():
    gb = QGroupBox("Goto Controls")
    
    #-------------------------------------------
    
    layout = QHBoxLayout()

    label = QLabel("Goto angle:")
    lineEdit = QLineEdit()
    go_btn = QPushButton("Go")
    
    lineEdit.setMaximumWidth(30)
    go_btn.clicked.connect(lambda: send_cmd(["g"+lineEdit.displayText()]))


    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)
    
    layout.addWidget(label)
    layout.addWidget(lineEdit)
    layout.addSpacing(10)
    layout.addWidget(go_btn)
    
    upper = QWidget()
    upper.setLayout(layout)
    
    #------------------------------------------
    
    layout = QHBoxLayout()

    btn0 = QPushButton("0")
    btn90 = QPushButton("90")
    btn180 = QPushButton("180")
    btn270 = QPushButton("270")
    
    btn0.clicked.connect(lambda: send_cmd(["g0"]))
    btn90.clicked.connect(lambda: send_cmd(["g90"]))
    btn180.clicked.connect(lambda: send_cmd(["g" + str(180+float(offAt180_sb.value()))]))
    btn270.clicked.connect(lambda: send_cmd(["g270"]))

    btn0.setMaximumWidth(40)
    btn90.setMaximumWidth(40)
    btn180.setMaximumWidth(40)
    btn270.setMaximumWidth(40)

    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)

    layout.addWidget(btn0)
    layout.addWidget(btn90)
    layout.addWidget(btn180)
    layout.addWidget(btn270)
    
    lower = QWidget()
    lower.setLayout(layout)
    
    #------------------------------------------
    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addWidget(lower)
    vbox.addStretch(1)
    
    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    gb.setLayout(vbox)
    return gb


def createControlWidgets():
    gb = QGroupBox()
    
    layout = QHBoxLayout()
    
    layout.addWidget(createSpeedControls())
    layout.addWidget(createCalibrationControls())
    layout.addWidget(createNavigationControls())
    
    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(5)

    upper = QWidget()
    upper.setLayout(layout)
    
    #------------------------------------------
    
    lower = createMonitorControls()

    #------------------------------------------

    vbox = QVBoxLayout()
    
    vbox.addWidget(upper)
    vbox.addWidget(lower)
    vbox.addStretch(1)
    
    vbox.setContentsMargins(0,0,0,0)
    vbox.setSpacing(0)

    right = QWidget()
    right.setLayout(vbox)
    
    #------------------------------------------
    
    layout = QHBoxLayout()
    
    layout.addStretch(1)
    layout.addWidget(right)

    layout.setContentsMargins(0,0,0,0)
    layout.setSpacing(0)
    
    gb.setLayout(layout)
    gb.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
    gb.setMaximumHeight(400)
    return gb


def height_slider_changed(height):
    print("Height = %d" % height)

def send_cmd(cmd_strs):
    if isinstance(cmd_strs, str):
        cmd_strs = [cmd_strs]
        
    for cmd_str in cmd_strs:
        s.write(bytes(cmd_str + "\n", "utf-8"))

def pause_vector_and_time():
    tracker.pause_time()
    send_cmd(["p"])

def continue_():
    send_cmd(["c"])
    tracker.unpause_time()
    

#======================================================================================================
# start main script
#======================================================================================================
app = QApplication(sys.argv)

window = QWidget()


vbox = QVBoxLayout()

tracker = ShadowTipTracker()
control_widgets = createControlWidgets()

vbox.addWidget(control_widgets)
vbox.addWidget(tracker)

window.setLayout(vbox)
window.setGeometry(5,50,1900,800)

# window = MainWindow()
window.show()


t = Thread(target=com_port_reader, daemon=True)
thread_done = False
t.start()

sys.exit(app.exec_())
